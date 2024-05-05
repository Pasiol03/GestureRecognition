#include "hmm.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstdlib>

void train_with_iteration_test(vector<HMM*>* HMM_HEAD, int iteration, string data_path);

void HMM::Softmax(vector<double>* props) {
	if (props->empty())
		return;

	double max_val = *std::max_element(props->begin(), props->end());

	double exp_sum = 0.0;
	for (double& val : *props) { // Dereference the pointer to iterate over the vector's elements
		val = std::exp(val - max_val);
		exp_sum += val;
	}

	for (double& val : *props) { // Dereference the pointer to iterate over the vector's elements
		val /= exp_sum;
	}
}

void HMM::debugLog() {
	cout << "Initial Vector PI:" << endl;
	for (int i = 0; i < NUM_STATES; ++i) { cout << P(i) << " "; }
	cout << endl;

	cout << "Transformation Matrix:" << endl;
	for (int i = 0; i < NUM_STATES; ++i) {
		for (int j = 0; j < NUM_STATES; ++j) { cout << A(i, j) << " "; }
		cout << endl;
	}

	cout << "Observation Matrix:" << endl;
	for (int i = 0; i < NUM_STATES; ++i) {
		for (int j = 0; j < NUM_OBSERVATIONS; ++j) { cout << B(i, j) << " "; }
		cout << endl;
	}
}

void HMM::train(HMM* hmm, int iteration, string modelDataPath) {
	cout << endl << "===============================================" << endl;
	cout << "Training new model: " << hmm->getName() << endl;
	vector<vector<char>> data = readData(modelDataPath);
	hmm->trainHMM(iteration, &data);
	hmm->dumpHMM(stderr, true);
	hmm->dumpHMM(openFile(hmm->getName().c_str(), "w"), false);
}

void train_with_iteration_test(vector<HMM*>* HMM_HEAD, int iteration, string data_path) {
	cout << endl << "===============================================" << endl;

	// initialize variables
	HMM* hmm;
	double acc;
	double max_acc = 0;
	int max_iteration = 0;

	// initialize files
	ofstream acc_file;
	acc_file.open("../data/training_log.txt");

	// read all data
	vector<vector<vector<char>>> datas;
	for (int m = 0; m < HMM_HEAD->size(); ++m) {
		hmm = HMM_HEAD->at(m);
		string model_data_path = data_path + "seq_" + hmm->getName();
		datas.push_back(readData(model_data_path));
	}
	vector<vector<char>> test_data = readData("../data/testing_data1.txt");

	// train with iteration-wise test
	for (int iter = 0; iter < iteration; iter++) {
		// train all models 1 iteration
		for (int m = 0; m < HMM_HEAD->size(); ++m) {
			cout << "\r" << "Computing model: " << m << '/' << HMM_HEAD->size() << "..." << flush;
			hmm = HMM_HEAD->at(m);
			hmm->trainHMM(1, &datas.at(m), false);
		}

		// test with all models
		acc = test(HMM_HEAD, &test_data, "../data/testing_result1.txt", false);
		acc_file << acc << ' ' << iter + 1 << '\n';
		cout << "\r" << "[HMM training] Iteration: " << iter + 1 << '/' << iteration << ", Accuracy: " << acc;

		// save best
		if (max_acc < acc) {
			max_acc = acc;
			max_iteration = iter + 1;
			dumpModels(HMM_HEAD);
			cout << " - Saved." << endl;
		}
		else cout << endl;
	}
	acc_file.close();
	cout << "\r" << "[HMM training] Iteration: " << iteration << '/' << iteration << ", Accuracy: " << acc
		<< "... Complete." << endl;
	cout << "[HMM training] Max Accuracy " << max_acc << " achieved at Iteration: " << max_iteration << endl;
}

void HMM::trainHMM(const int& iteration, vector<vector<char>>* data, bool printLog) {
	vector<char>* obs;

	for (int iter = 0; iter < iteration; iter++) {
		if (printLog) {
			FString Message = FString::Printf(TEXT("HMM training: %d/%d..."), iter + 1, iteration);
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, Message);
		}

		// accumulaters
		vector<double>* pi_accumulate = new vector<double>(NUM_STATES, 0.0);
		vector<double>* gamma_accumulate = new vector<double>(NUM_STATES, 0.0);
		vector<vector<double>>* epsilon_accumulate = new vector<vector<double>>(NUM_STATES, vector<double>(NUM_STATES, 0.0));
		vector<vector<double>>* B_num_accumulate = new vector<vector<double>>(NUM_STATES, vector<double>(NUM_OBSERVATIONS, 0.0));
		vector<double>* B_den_accumulate = new vector<double>(NUM_STATES, 0.0);

		// iterate through all observation sequence in training data
		for (int s = 0; s < data->size(); ++s) {
			obs = &data->at(s);

			// computations
			vector<vector<double>>* alpha = computeAlpha(obs);
			vector<vector<double>>* beta = computeBeta(obs);
			vector<vector<double>>* gamma = computeGamma(alpha, beta);
			vector<vector<vector<double>>>* epsilon = computeEpsilon(obs, alpha, beta);

			// update
			sumUpdate(obs, gamma, epsilon, pi_accumulate, gamma_accumulate, epsilon_accumulate,
			                  B_num_accumulate, B_den_accumulate);
			delete alpha;
			delete beta;
			delete gamma;
			delete epsilon;
		}
		applyUpdate(pi_accumulate, gamma_accumulate, epsilon_accumulate, B_num_accumulate, B_den_accumulate,
		             (double)data->size());
		delete pi_accumulate;
		delete gamma_accumulate;
		delete epsilon_accumulate;
		delete B_num_accumulate;
		delete B_den_accumulate;
	}
	if (printLog) {
		FString Message = FString::Printf(TEXT("HMM training: %d/%d... Complete."), iteration, iteration);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Message);
	}
}


vector<vector<double>>* HMM::computeAlpha(vector<char>* obs) const {
	vector<vector<double>>* alpha = new vector<vector<double>>(T, vector<double>(NUM_STATES, 0.0));
	for (int i = 0; i < NUM_STATES; ++i) { alpha(0, i) = P(i) * B(i, o(0)); }
	for (int t = 0; t < T - 1; ++t)
		for (int i = 0; i < NUM_STATES; ++i) {
			for (int j = 0; j < NUM_STATES; ++j)
				alpha(t + 1, i) += alpha(t, j) * A(j, i);
			alpha(t + 1, i) *= B(i, o(t + 1));
		}
	return alpha;
}


vector<vector<double>>* HMM::computeBeta(vector<char>* obs) const {
	vector<vector<double>>* beta = new vector<vector<double>>(T, vector<double>(NUM_STATES, 0.0));
	for (int i = 0; i < NUM_STATES; ++i)
		beta(T - 1, i) = 1.0;
	for (int t = T - 2; t >= 0; --t)
		for (int i = 0; i < NUM_STATES; ++i)
			for (int j = 0; j < NUM_STATES; ++j)
				beta(t, i) += A(i, j) * B(j, o(t + 1)) * beta(t + 1, j);
	return beta;
}


vector<vector<double>>* HMM::computeGamma(vector<vector<double>>* alpha, vector<vector<double>>* beta) const {
	vector<vector<double>>* gamma = new vector<vector<double>>(alpha->size(), vector<double>(NUM_STATES, 0.0));
	for (int t = 0; t < alpha->size(); ++t) {
		double gamma_base = 0.0;
		for (int i = 0; i < NUM_STATES; ++i) {
			gamma(t, i) = (alpha(t, i) * beta(t, i));
			gamma_base += gamma(t, i);
		}
		for (int i = 0; i < NUM_STATES; ++i)
			gamma(t, i) /= gamma_base;
	}
	return gamma;
}


vector<vector<vector<double>>>*
HMM::computeEpsilon(vector<char>* obs, vector<vector<double>>* alpha, vector<vector<double>>* beta) const {
	vector<vector<vector<double>>>* epsilon = new vector<vector<vector<double>>>(T - 1, vector<vector<double>>(NUM_STATES,
		vector<double>(
			NUM_STATES,
			0.0)));
	for (int t = 0; t < T - 1; ++t) {
		double epsilon_base = 0.0;
		for (int i = 0; i < NUM_STATES; ++i)
			for (int j = 0; j < NUM_STATES; ++j) {
				epsilon(t, i, j) = alpha(t, i) * A(i, j) * B(j, o(t + 1)) * beta(t + 1, j);
				epsilon_base += epsilon(t, i, j);
			}
		for (int i = 0; i < NUM_STATES; ++i)
			for (int j = 0; j < NUM_STATES; ++j)
				epsilon(t, i, j) /= epsilon_base;
	}
	return epsilon;
}


void HMM::sumUpdate(vector<char>* obs,
                            vector<vector<double>>* gamma,
                            vector<vector<vector<double>>>* epsilon,
                            vector<double>* piSum,
                            vector<double>* gammaSum,
                            vector<vector<double>>* epsilonSum,
                            vector<vector<double>>* B_num_accumulate,
                            vector<double>* obsEpsilonSum) const {
	for (int i = 0; i < NUM_STATES; ++i) {
		piSum->at(i) += gamma(0, i);
		for (int t = 0; t < T - 1; ++t) {
			gammaSum->at(i) += gamma(t, i);
			for (int j = 0; j < NUM_STATES; ++j)
				epsilonSum->at(i).at(j) += epsilon(t, i, j);
		}
	}
	for (int i = 0; i < NUM_STATES; ++i)
		for (int t = 0; t < T; ++t) {
			//            cout << o(t) << " ";
			B_num_accumulate->at(i).at(o(t)) += gamma(t, i);
			obsEpsilonSum->at(i) += gamma(t, i);
		}
	//    cout <<endl;
}


void HMM::applyUpdate(vector<double>* piSum,
                       vector<double>* gammaSum,
                       vector<vector<double>>* epsilonSum,
                       vector<vector<double>>* obsSumPerState,
                       vector<double>* obsEpsilonSum,
                       double numSample) {
	// update pi
	for (int i = 0; i < NUM_STATES; i++)
		P(i) = piSum->at(i) / numSample;
	// update _transition
	for (int i = 0; i < NUM_STATES; ++i)
		for (int j = 0; j < NUM_STATES; ++j)
			A(i, j) = epsilonSum->at(i).at(j) / gammaSum->at(i);
	// update _observation
	for (int i = 0; i < NUM_STATES; ++i)
		for (int t = 0; t < NUM_OBSERVATIONS; ++t)
			B(i, t) = obsSumPerState->at(i).at(t) / obsEpsilonSum->at(i);
}

// Function to generate a random normalized matrix
std::vector<std::vector<double>> generateNormalizedMatrix(int rows, int cols) {
	std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols));

	// Fill the matrix with even normalized distribution
	for (int i = 0; i < rows; ++i) { for (int j = 0; j < cols; ++j) { matrix[i][j] = 1.0 / cols; } }

	return matrix;
}

// Function to save the HMM to a file
void saveHMMToFile(int numStates, int numObservations, const std::vector<std::vector<double>>& initial,
                   const std::vector<std::vector<double>>& transition,
                   const std::vector<std::vector<double>>& observation,
                   const std::string& fileName) {
	std::ofstream outFile(fileName, std::ios::trunc); // Open the file in truncate mode

	if (outFile.is_open()) {
		// Save the initial distribution
		outFile << "initial: " << numStates << std::endl;
		for (int i = 0; i < numStates; ++i) {
			double value = initial[0][i];
			if (value < 0.0) value = 0.05; // Set to 0.05 if negative
			outFile << std::fixed << std::setprecision(5) << value << " ";
		}
		outFile << std::endl << std::endl;

		// Save the transition matrix
		outFile << "transition: " << numStates << std::endl;
		for (int i = 0; i < numStates; ++i) {
			// Find the first non-zero value in the column
			double nonZeroValue = 0.0;
			for (int j = 0; j < numStates; ++j) {
				if (transition[j][i] != 0.0) {
					nonZeroValue = std::abs(transition[j][i]);
					break;
				}
			}

			for (int j = 0; j < numStates; ++j) {
				// Set to 0.05 if negative
				double value = (nonZeroValue < 0.0) ? 0.05 : nonZeroValue;
				outFile << std::fixed << std::setprecision(5) << value << " ";
			}
			outFile << std::endl;
		}
		outFile << std::endl;

		// Save the observation matrix
		outFile << "observation: " << numObservations << std::endl;
		for (int i = 0; i < numStates; ++i) {
			for (int j = 0; j < numObservations; ++j) {
				double value = observation[i][j];
				if (value <= 0.0) value = 0.05; // Set to 0.05 if negative
				outFile << std::fixed << std::setprecision(5) << value << " ";
			}
			outFile << std::endl;
		}

		outFile.close();
		std::cout << "HMM saved to " << fileName << std::endl;
	}
	else { std::cerr << "Error opening file: " << fileName << std::endl; }
}
