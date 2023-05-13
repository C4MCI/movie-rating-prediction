//
// Created by semih on 1/4/23.
//

#ifndef TERMP4_FUNCTIONS_H
#define TERMP4_FUNCTIONS_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <set>
#include <sstream>
#include <cerrno>
#include <unordered_map>
#include <cmath>
#include <bits/stdc++.h>
#include <stdexcept>


using namespace std;

vector<vector<string>> read_csv_train() {

    fstream file("/home/semih/Documents/termp2/train.csv", ios::in);
    vector<vector<string>> content;
    vector<string> row;
    string line, word;

    if (file.fail()) {
        perror("open failed ");
    }

    while (getline(file, line)) {
        row.clear();
        stringstream s(line);
        while (getline(s, word, ',')) {
            row.push_back(word);
        }
        content.push_back(row);
    }

    return content;
}

vector<vector<string>> read_csv_test() {

    fstream file("/home/semih/Documents/termp2/test.csv", ios::in);
    vector<vector<string>> content;
    vector<string> row;
    string line, word;

    if (file.fail()) {
        perror("open failed ");
    }

    while (getline(file, line)) {
        row.clear();
        stringstream s(line);
        while (getline(s, word, ',')) {
            row.push_back(word);
        }
        content.push_back(row);
    }

    return content;
}



unordered_map<int, double> get_means(unordered_map<int, unordered_map<int, double>> &matrix) {
    unordered_map<int, double> meanValues;
    for (auto &i: matrix) {
        double sum = 0;
        double count = 0;
        for (auto &j: i.second) {
            if (!isnan(j.second)) {
                sum += j.second;
                count++;
            }
        }
        meanValues[i.first] = sum / count;
    }
    return meanValues;
}


double pearsonCorr(vector<double> X, vector<double> Y, unsigned int n) {

    double sum_X = 0.0, sum_Y = 0.0, sum_XY = 0.0;
    double squareSum_X = 0.0, squareSum_Y = 0.0;

    for (int i = 0; i < n; i++) {

        sum_X = sum_X + X[i];


        sum_Y = sum_Y + Y[i];


        sum_XY = sum_XY + X[i] * Y[i];


        squareSum_X = squareSum_X + X[i] * X[i];
        squareSum_Y = squareSum_Y + Y[i] * Y[i];
    }


    double corr = (double) (n * sum_XY - sum_X * sum_Y)
                  / sqrt((n * squareSum_X - sum_X * sum_X)
                         * (n * squareSum_Y - sum_Y * sum_Y));

    if (corr < 0)
        corr = 0.0;

    if (isnan(corr))
        corr = 0.0;

    return corr;
}

pair<int, double> get_correlation(int user1, int user2, unordered_map<int, unordered_map<int, double>> &movieMatrix) {
    vector<double> user1_ratings;
    vector<double> user2_ratings;
    for (auto &i: movieMatrix[user1]) {
        for (auto &j: movieMatrix[user2]) {
            if (i.first == j.first) {
                user1_ratings.push_back(i.second);
                user2_ratings.push_back(j.second);
            }
        }
    }

    double corr;
    if (user1_ratings.size() >= 10) {
        corr = pearsonCorr(user1_ratings, user2_ratings, user1_ratings.size());
    } else {
        corr = 0;
    }
    pair<int, double> s;
    s.first = user2;
    s.second = corr;

    return s;

}

vector<int> get_similars(int itemId, unordered_map<int, unordered_map<int, double>> &itemMatrix) {
    vector<int> similar_users;
    for (auto &i: itemMatrix[itemId]) {
        similar_users.push_back(i.first);
    }

    return similar_users;
}

bool comparePairs(pair<int, double> i1, pair<int, double> i2) {
    return (i1.second > i2.second);
}


double predict(int userId, int itemId, unordered_map<int, unordered_map<int, double>> &itemMatrix,
               unordered_map<int, unordered_map<int, double>> &movieMatrix,
               unordered_map<int, double> &meanValues_item, unordered_map<int, double> &meanValues_user) {
    vector<pair<int, double>> correlations;
    vector<int> similar_users = get_similars(itemId, itemMatrix);
    for (auto &i: similar_users) {
        correlations.push_back(get_correlation(userId, i, movieMatrix));
    }
    double pred;
    if (correlations.empty()) {
        pred = ((meanValues_item[itemId] + meanValues_user[userId]) / 2);
    } else {
        sort(correlations.begin(), correlations.end(), comparePairs);
        double numerator = 0.0;
        double denominator = 0.0;
        for (auto &i: correlations) {
            if (i.second >= 0.4) {
                numerator += (movieMatrix[i.first][itemId] - meanValues_user[i.first]) * i.second;
                denominator += i.second;
            }
        }
        pred = ((numerator / denominator) + (meanValues_item[itemId] + meanValues_user[userId]) / 2);

        if (denominator == 0) {
            pred = ((meanValues_item[itemId] + meanValues_user[userId]) / 2);
        }

        if (pred > 5)
            pred = 5;
        if (pred < 0)
            pred = 0;
    }
    return pred;
}


#endif //TERMP4_FUNCTIONS_H
