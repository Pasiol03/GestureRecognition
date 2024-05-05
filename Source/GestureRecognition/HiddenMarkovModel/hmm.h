//
// Created by pascal on 06.01.2024.
//
#ifndef HMM_HEADER_
#define HMM_HEADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <float.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cassert>

using namespace std;

#ifndef MAX_STATE
#define MAX_STATE    32
#endif
#ifndef MAX_OBSERV
#define MAX_OBSERV    128
#endif
#ifndef MAX_SEQ
#define    MAX_SEQ        200
#endif
#ifndef MAX_LINE
#define MAX_LINE    256
#endif
#ifndef NUM_MODEL
#define NUM_MODEL    5
#endif
#ifndef NUM_STATES
#define NUM_STATES stateNum
#endif
#ifndef NUM_OBSERVATIONS
#define NUM_OBSERVATIONS observNum
#endif
#ifndef P
#define P(i) IntialVector.at(i)                            // shape: (max state)
#endif
#ifndef A
#define A(i, j) TransitionMatrix.at(i).at(j)                // shape: (max state, max state)
#endif
#ifndef B
#define B(s, o) ObservationMatrix.at(s).at(o)                // shape: (max state, max observation)
#endif
#ifndef alpha
#define alpha(t, s) alpha->at(t).at(s)                    // shape: (max sequence len T, num state)
#endif
#ifndef beta
#define beta(t, s) beta->at(t).at(s)                    // shape: (max sequence len T, num state)
#endif
#ifndef gamma
#define gamma(t, s) gamma->at(t).at(s)                    // shape: (max sequence len T, num state)
#endif
#ifndef epsilon
#define epsilon(t, i, j) epsilon->at(t).at(i).at(j)        // shape: (max sequence len T, num state)
#endif
#ifndef delta
#define delta(t, s) delta->at(t).at(s)                    // shape: (max sequence len T, num state)
#endif
#ifndef o
#define o(t) (obs->at(t) % 127)

#endif
#ifndef T
#define T obs->size()                                    // single observation sample length
#endif

class HMM {
public:
	HMM(bool max_allocate = false) {
		if (max_allocate) Initialize();
		maxAllocate = max_allocate;
	};

	static void StartTrainingProcess();

	static void TestRealTime(vector<char>* Sequence);

	static void Softmax(vector<double>* props);

	void loadHMM(const char* filename);

	void loadLocalHMM(vector<double> initialMatrix, vector<vector<double>> transitionMatrix,
	                  vector<vector<double>> observationMatrix);

	void dumpHMM(FILE* fp, bool display = false) const;

	string getName() const { return modelName; }

	void setName(const char* name) {
		string str(name);
		modelName = str;
	}

	void trainHMM(const int& iteration, vector<vector<char>>* data, bool printLog = true);

	double testHMM(vector<char>* obs) const;

	double testHMM(vector<uint8>* obs) const;

	void debugLog();

	double viterbi(HMM* hmm, std::string& dataPath);

	static void generateTestFiles();

	static void train(HMM* hmm, int iteration, string modelDataPath);

private:
	void Initialize();

	vector<vector<double>>* computeAlpha(vector<char>* obs) const;

	vector<vector<double>>* computeBeta(vector<char>* obs) const;

	vector<vector<double>>* computeGamma(vector<vector<double>>* alpha, vector<vector<double>>* beta) const;

	vector<vector<vector<double>>>*
	computeEpsilon(vector<char>* obs, vector<vector<double>>* alpha, vector<vector<double>>* beta) const;

	void sumUpdate(vector<char>* obs,
	                       vector<vector<double>>* gamma,
	                       vector<vector<vector<double>>>* epsilon,
	                       vector<double>* piSum,
	                       vector<double>* gammaSum,
	                       vector<vector<double>>* epsilonSum,
	                       vector<vector<double>>* obsSumPerState,
	                       vector<double>* obsEpsilonSum) const;

	void applyUpdate(vector<double>* piSum,
	                  vector<double>* gammaSum,
	                  vector<vector<double>>* epsilonSum,
	                  vector<vector<double>>* obsSumPerState,
	                  vector<double>* obsEpsilonSum,
	                  double numSample);

	bool maxAllocate; // allocate maximum memory
	string modelName; // model name
	int stateNum; // number of state
	int observNum; // number of observation
	vector<double> IntialVector; // initial prob, shape: (max state)
	vector<vector<double>> TransitionMatrix; // transition prob, shape: (max state, max state)
	vector<vector<double>> ObservationMatrix; // observation prob, shape: (max state, max observation)

public:
	[[nodiscard]] vector<double>& Initial() { return IntialVector; }
	[[nodiscard]] vector<vector<double>>& Transition() { return TransitionMatrix; }
	[[nodiscard]] vector<vector<double>>& Observation() { return ObservationMatrix; }
};

double test(vector<HMM*>* HMM_HEAD, vector<vector<char>>* data, const string& outputPath, bool printLog = true);

double computeAcc(const string& answerPath, const string& resultPath);

FILE* openFile(const char* filename, const char* ht);

vector<vector<char>> readData(const string& dataPath);

vector<HMM*>* loadModels(const char* modelList, const int maxNum);

vector<HMM*>* initializeNewModels(const char* modelInit, const char* modelList, const int maxNum);

void dumpModels(vector<HMM*>* HMM_HEAD, bool to_file = true);

std::vector<std::vector<double>> generateNormalizedMatrix(int rows, int cols);

void saveHMMToFile(int numStates, int numObservations, const std::vector<std::vector<double>>& initial,
                   const std::vector<std::vector<double>>& transition,
                   const std::vector<std::vector<double>>& observation,
                   const std::string& fileName);


#endif //TEST_HMM_H
