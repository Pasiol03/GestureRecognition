#include "hmm.h"
#include <string.h>
#include <cstring>

using namespace std;

void HMM::Initialize() {
    IntialVector = vector<double>(MAX_STATE, FLT_MAX);
    TransitionMatrix = vector<vector<double>>(MAX_STATE, vector<double>(MAX_STATE, FLT_MAX));
    ObservationMatrix = vector<vector<double>>(MAX_STATE, vector<double>(128, FLT_MAX));
}

void HMM::loadHMM(const char *filename) {
    int i, j;
    int f;
    
    FILE *fp = openFile(filename, "r");
   
    string str(filename);
    modelName = str;

    char token[MAX_LINE] = "";
    while (fscanf(fp, "%s", token) > 0) {
            if (token[0] == '\0' || token[0] == '\n') continue;

            if (strcmp(token, "initial:") == 0) {
                f = fscanf(fp, "%d", &stateNum);
                if (!maxAllocate) IntialVector = std::vector<double>(stateNum, DBL_MAX);
                for (i = 0; i < stateNum; i++)
                    f = fscanf(fp, "%lf", &(IntialVector[i]));
            } else if (strcmp(token, "transition:") == 0) {
                f = fscanf(fp, "%d", &stateNum);
                if (!maxAllocate) TransitionMatrix = std::vector<std::vector<double>>(stateNum, std::vector<double>(stateNum, DBL_MAX));
                for (i = 0; i < stateNum; i++)
                    for (j = 0; j < stateNum; j++)
                        f = fscanf(fp, "%lf", &(TransitionMatrix[i][j]));
            } else if (strcmp(token, "observation:") == 0) {
                f = fscanf(fp, "%d", &observNum);
                if (!maxAllocate) ObservationMatrix = std::vector<std::vector<double>>(stateNum, std::vector<double>(observNum, DBL_MAX));
                for (i = 0; i < observNum; i++)
                    for (j = 0; j < stateNum; j++) {
                        f = fscanf(fp, "%lf", &(ObservationMatrix[j][i]));
                        if (ObservationMatrix[j][i] > 1) {
                            ObservationMatrix[j][i] = 0.000001;
                        }
                    }
            }
        }
    debugLog();
}

void HMM::loadLocalHMM(vector<double> initialMatrix, vector<vector<double>> transitionMatrix,
                       vector<vector<double>> observationMatrix) {
    IntialVector = initialMatrix;
    TransitionMatrix = transitionMatrix;
    ObservationMatrix = observationMatrix;
    debugLog();
}

void HMM::dumpHMM(FILE *fp, bool display) const {
    int i, j;

    if (display) {
        fprintf(fp, "===============================================\n");
        fprintf(fp, "model name: %s\n", modelName.c_str());
    }
    fprintf(fp, "initial: %d\n", stateNum);
    for (i = 0; i < stateNum - 1; i++)
        fprintf(fp, "%.17g ", IntialVector[i]);
    fprintf(fp, "%.17g\n", IntialVector[stateNum - 1]);

    fprintf(fp, "\ntransition: %d\n", stateNum);
    for (i = 0; i < stateNum; i++) {
        for (j = 0; j < stateNum - 1; j++)
            fprintf(fp, "%.17g ", TransitionMatrix[i][j]);
        fprintf(fp, "%.17g\n", TransitionMatrix[i][stateNum - 1]);
    }
    fprintf(fp, "\nobservation: %d\n", observNum);

    for (i = 0; i < stateNum - 1; i++) {
        for (j = 0; j < observNum - 1; j++)
            fprintf(fp, "%.17g ", ObservationMatrix[i][j]);
        fprintf(fp, "%.17g\n", ObservationMatrix[i][observNum - 1]);
    }
}

double HMM::testHMM(vector<char> *obs) const {
    // viterbi algorithm
    auto *delta = new vector<vector<double>>(T, vector<double>(NUM_STATES, 0));
    for (int i = 0; i < NUM_STATES; ++i) {
        delta(0, i) = P(i) * B(i, o(0));
    }
    for (int t = 0; t < T - 1; ++t)
        for (int j = 0; j < NUM_STATES; ++j) {
            for (int i = 0; i < NUM_STATES; ++i)
                if (delta(t + 1, j) < (delta(t, i) * A(i, j)))
                    delta(t + 1, j) = delta(t, i) * A(i, j);
            delta(t + 1, j) *= B(j, o(t + 1));
        }
    double viterbi_prob = 0;
    for (int i = 0; i < NUM_STATES; ++i){
        cout << delta(T-1, i) << " " << endl;
        if (viterbi_prob < delta(T - 1, i))
            viterbi_prob = delta(T - 1, i);
    }
    delete delta;
    return viterbi_prob;
}

double HMM::testHMM(vector<uint8>* obs) const {
    auto *delta = new vector<vector<double>>(T, vector<double>(NUM_STATES, 0));
    for (int i = 0; i < NUM_STATES; ++i) {
        delta(0, i) = P(i) * B(i, (*obs)[0]);
    }
    for (int t = 0; t < T - 1; ++t)
        for (int j = 0; j < NUM_STATES; ++j) {
            for (int i = 0; i < NUM_STATES; ++i)
                if (delta(t + 1, j) < (delta(t, i) * A(i, j)))
                    delta(t + 1, j) = delta(t, i) * A(i, j);
            delta(t + 1, j) *= B(j, (*obs)[t + 1]);
        }
    double viterbi_prob = 0;
    for (int i = 0; i < NUM_STATES; ++i){
        cout << delta(T-1, i) << " " << endl;
        if (viterbi_prob < delta(T - 1, i))
            viterbi_prob = delta(T - 1, i);
    }
    delete delta;
    return viterbi_prob;
}

double HMM::viterbi(HMM *hmm, std::string &dataPath) {
    vector<vector<char>> data = readData(dataPath);
    vector<char> *obs;

    for (int i = 0; i < data.size(); ++i) {
        if (true) cout << "\r" << "HMM testing: " << i << '/' << data.size() << "... " << flush;
        obs = &data.at(i);
        double prop = hmm->testHMM(obs);
        cout << " Probability sequence: " << prop << endl;
    }
    return 0;

}

void HMM::generateTestFiles() {
    // Empty the contents of the files
    ofstream data_file("F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/testing_data.txt", ios::trunc);
    data_file.close();

    ofstream answer_file("F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/testing_answer.txt", ios::trunc);
    answer_file.close();

    srand(time(0));
    vector<vector<char>> test_data1 = readData("F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/test/Sequence_Fig1_Test.txt");
    vector<vector<char>> test_data2 = readData("F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/test/Sequence_Fig2_Test.txt");
    for (int i = 0; i < 100; ++i) {
        int random_value = rand() % 2;
        vector<char> *obs;

        // Decide randomly between test_data1 and test_data2
        if (random_value == 0) {
            obs = &test_data1[rand() % test_data1.size()];
        } else {
            obs = &test_data2[rand() % test_data2.size()];
        }

        // Save data to testing_data.txt
        ofstream File("F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/testing_data.txt", ios::app);
        for (char ch: *obs) {
            File << ch;
        }
        File << endl;
        File.close();

        // Save answer to testing_answer.txt
        ofstream Answer_File("F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/testing_answer.txt", ios::app);
        if (random_value == 0) {
            Answer_File << "F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/trained/model_01.txt";
        } else {
            Answer_File << "F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/trained/model_02.txt";
        }
        Answer_File << endl;  // Linebreak after writing the answer
        Answer_File.close();
    }
}

double test(vector<HMM *> *HMM_HEAD, vector<vector<char>> *data, const string &outputPath, bool printLog) {

    vector<double> *max_prob = new vector<double>(data->size(), 0);
    vector<int> *model = new vector<int>(data->size(), 0);
    vector<char> *obs;

    for (int i = 0; i < data->size(); ++i) {
        if (printLog) cout << "\r" << "HMM testing: " << i << '/' << data->size() << "... " << flush;
        obs = &data->at(i);

        double prob = 0;
        for (int m = 0; m < HMM_HEAD->size(); ++m) {
            prob = HMM_HEAD->at(m)->testHMM(obs);
            if (max_prob->at(i) < prob) {
                max_prob->at(i) = prob;
                model->at(i) = m;
            }
        }
    }
    if (printLog)
        cout << "\r" << "HMM testing: " << data->size() << '/' << data->size() << "... " << " Complete." << endl;

    ofstream file;
    file.open(outputPath.c_str());
    for (int i = 0; i < data->size(); ++i)
        file << HMM_HEAD->at(model->at(i))->getName() << ' ' << max_prob->at(i) << '\n';
    file.close();

    double acc = computeAcc("F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/testing_answer.txt", outputPath);
    if (printLog) cout << "Testing accuracy: " << acc << endl;
    delete max_prob;
    delete model;
    return acc;
}

double computeAcc(const string &answerPath, const string &resultPath) {
    // Opening files
    ifstream ifs_ans(answerPath.c_str());
    ifstream ifs_res(resultPath.c_str());
    assert(ifs_ans);
    assert(ifs_res);

    // Read file paths
    string str_buf;
    vector<string> answer;
    vector<string> result;

    while (getline(ifs_ans, str_buf)) {
        // Extract the file path
        string ans = str_buf.substr(0, str_buf.find(" "));
        answer.push_back(ans);
    }

    while (getline(ifs_res, str_buf)) {
        // Extract the file path
        string res = str_buf.substr(0, str_buf.find(" "));
        result.push_back(res);
    }

    // Compute score
    assert(answer.size() == result.size());
    double acc = 0;

    for (int i = 0; i < answer.size(); ++i) {
        if (answer.at(i) == result.at(i)) {
            acc += 1;
        }
    }
    return (acc / static_cast<double>(answer.size()));
}

FILE *openFile(const char *filename, const char *ht) {
    FILE *fp = fopen(filename, ht);
    if (fp == NULL) {
        perror(filename);
        exit(1);
    }
    return fp;
}

vector<vector<char>> readData(const string &dataPath) {
    // opening file
    std::ifstream ifs(dataPath, std::ios::binary);
    assert(ifs.is_open());

    // Read file
    std::vector<std::vector<char>> data;
    const int max_line_length = 1024;  // Adjust as needed
    while (true) {
        char line[max_line_length];
        ifs.getline(line, max_line_length);

        if (ifs.eof()) {
            break;
        }
        int len = std::strlen(line);
        if (len > 0 && line[len - 1] == '\r') {
            line[len - 1] = '\0';
        }

        std::vector<char> char_line(line, line + len);
        data.push_back(char_line);
    }
    ifs.close();
    return data;
}


vector<HMM *> *loadModels(const char *modelList, const int maxNum) {
    FILE *fp = openFile(modelList, "r");

    int count = 0;
    char model_name[MAX_LINE] = "";
    vector<HMM *> *HMM_HEAD = new vector<HMM *>();
    HMM *hmm;

    while (fscanf(fp, "%s", model_name) == 1) {
        hmm = new HMM();
        hmm->loadHMM(model_name);
        HMM_HEAD->push_back(hmm);
        ++count;
        if (count >= maxNum)
            return HMM_HEAD;
    }
    fclose(fp);
    return HMM_HEAD;
}

vector<HMM *> *initializeNewModels(const char *modelInit, const char *modelList, const int maxNum) {
    FILE *fp = openFile(modelList, "r");

    int count = 0;
    char model_name[MAX_LINE] = "";
    vector<HMM *> *HMM_HEAD = new vector<HMM *>();
    HMM *hmm;

    while (fscanf(fp, "%s", model_name) == 1) {
        hmm = new HMM();
        hmm->loadHMM(modelInit);
        hmm->setName(model_name);
        HMM_HEAD->push_back(hmm);
        ++count;
        if (count >= maxNum)
            return HMM_HEAD;
    }
    fclose(fp);
    return HMM_HEAD;
}

void dumpModels(vector<HMM *> *HMM_HEAD, bool to_file) {
    for (int i = 0; i < HMM_HEAD->size(); ++i) {
        if (to_file) {
            HMM_HEAD->at(i)->dumpHMM(openFile(HMM_HEAD->at(i)->getName().c_str(), "w"), false);
        } else
            HMM_HEAD->at(i)->dumpHMM(stderr, true);
    }
}


