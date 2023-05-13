#include <iostream>
#include "functions.h"

using namespace std;

int main() {
    vector<std::vector<string>> train_content = read_csv_train();
    vector<vector<string>> test_content = read_csv_test();
    unordered_map<int, unordered_map<int, double>> movieMatrix;
    unordered_map<int, unordered_map<int, double>> itemMatrix;
    unordered_map<int, double> meanValues;
    unordered_map<int, double> meanValues_item;


    for (int i = 1; i < train_content.size(); ++i) {
        int userId = stoi(train_content[i][0]);
        int itemId = stoi(train_content[i][1]);
        double rating = stod(train_content[i][2]);
        movieMatrix[userId][itemId] = rating;
    }

    for (int i = 1; i < train_content.size(); ++i) {
        int itemId = stoi(train_content[i][1]);
        int userId = stoi(train_content[i][0]);
        double rating = stod(train_content[i][2]);
        itemMatrix[itemId][userId] = rating;
    }


    meanValues = get_means(movieMatrix);

    meanValues_item = get_means(itemMatrix);



    ofstream submission;
    submission.open("submission.csv");
    submission << "ID,Predicted\n";
    test_content.erase(test_content.begin());
    for (auto &i: test_content) {
        double pred = predict(stoi(i[1]), stoi(i[2]), itemMatrix, movieMatrix,
                              meanValues_item, meanValues);
        cout << i[0] << " " << pred << endl;
        submission << i[0] << "," << pred << "\n";
    }

    submission.close();


    return 0;
}
