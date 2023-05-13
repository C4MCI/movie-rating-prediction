# Movie Rating Prediction
<hr/>

# Correlations and Mean Values
### Finding mean rating of each user

```cpp
unordered_map<int, double> get_means(unordered_map<int, vector<pair<int, double>>> matrix) {
    unordered_map<int, double> meanValues;
    for (auto &i: matrix) {
        double sum = 0;
        double count = 0;
        for (auto &j: i.second) {
            sum += j.second;
            count++;
        }
        meanValues[i.first] = sum / count;
    }
    return meanValues;
}
```

In get_means() function, there is one parameter which is "&matrix". "matrix" is a hashmap with hashmap in it. First, we create
"meanValues" hashmap for storing datas. We are looking for the sum of the items rated by user, divided by the number of
ratings. So we find the mean values and the function get_means returns meanValues which has int as UserID, double as mean
rating

### Finding Pearson Correlation for two vectors

```cpp
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
```

pearsonCorr() function simply finds the Pearson Correlation for two given vectors. We have two vectors and one integer as
parameters. Two vectors for correlation calculation, integer for the size of the vectors.

### Finding Pearson Correlation for the users which has similar rated movies

```cpp
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
```

In get_correlation() function, there are two integers and one matrix as parameters. We are creating two vectors for storing movies'
ratings for user1 and second vector for user2. At the end of the function, if the "user1_ratings" vector's size is equal or greater
than 10, we give the corr variable Pearson Correlation of user1_ratings and user2_ratings vectors. If not, we give zero to corr.
And our function returns pair, which has int as key(userID) and double(pearson correlation score) as value.


# Prediction


### Finding users that rated the same movie
```cpp
vector<int> get_similars(int itemId, unordered_map<int, unordered_map<int, double>> &itemMatrix) {
    vector<int> similar_users;
    for (auto &i: itemMatrix[itemId]) {
        similar_users.push_back(i.first);
    }

    return similar_users;
}
```

The function above goes over all the users that rated the given item, adds them to a vector of integers, and returns that vector. We will use that function to make a better prediction below.



### Sorting the correlation
```cpp
bool comparePairs(pair<int, double> i1, pair<int, double> i2) {
    return (i1.second > i2.second);
}
```
This function is used to sort correlation coefficients with the std::sort() function below.



## Predicting the Rating
```cpp
double predict (int userId, int itemId, unordered_map<int, unordered_map<int, double>> &itemMatrix,
               unordered_map<int, unordered_map<int, double>> &movieMatrix,
               unordered_map<int, double> &meanValues_item,
                unordered_map<int, double> &meanValues_user)
```

In our function for prediction, we take 6 values as inputs, as shown in the code above. Our main goal is to make a viable prediction using our similarity function (that we explained above) and these inputs.

```cpp
    vector<pair<int, double>> correlations;
    vector<int> similar_users = get_similars(userId, itemId, itemMatrix);
    for (auto &i: similar_users) {
        correlations.push_back(get_correlation(userId, i, movieMatrix));
    }
```

We get the users that have rated our movie by using the get_similars function (explained above) and put them in a vector. Then we go over this vector with a for loop to get their correlation with our user if they have more than 10 common movies (get_correlation function explained above) and put them in a vector of pairs. The first variable in the pair is the id number of the user and the second variable is its correlation with our user in function input.

```cpp
    double pred;
    if (correlations.empty()) {
        pred = ((meanValues_item[itemId] + meanValues_user[userId]) / 2);
    }
```

If there are no users that have more than 10 common movies with our user, then we make our prediction by getting the mean of our user's mean rating and our movie's mean rating.

```cpp
    else {
        sort(correlations.begin(), correlations.end(), comparePairs);
        double numerator = 0.0;
        double denominator = 0.0;
        for (auto &i: correlations) {
            if (i.second >= 0.4) {
                numerator += (movieMatrix[i.first][itemId] - meanValues_user[i.first]) * i.second;
                denominator += i.second;
            }
        }
```


If we have similar users, we first sort them in descending order using our custom sort function explained above to find the users with the biggest correlation coefficients. After that, we go over them with a for loop and check their correlation values. If the correlation value is bigger than or equal to 0.4 we add the following value to the numerator:
((Rating of similar user to our item) - (mean rating of the similar user)) * correlation.

The reason we subtract the mean rating is to improve our prediction's precision. For example, two users might find a movie average. Which means they are similar. But let the first user be a person that gives 5 stars to a movie that he/she thinks is average. Being average might be good enough for that user to give 5 stars to it. So by removing the mean ratings of each user, we aim to remove that bias factor.

The reason we multiply with correlation coefficient is to simply give ratings a weight so users with high coefficient value have a higher weight in the result.

```cpp
    pred = (numerator / denominator) + meanValues_user[userId];
```

We make our prediction by adding the mean rating of our user to the result because we found or result by removing the mean rating bias. Adding our user's mean rating to our result will yield our prediction.

```cpp
    if (denominator == 0) {
            pred = ((meanValues_item[itemId] + meanValues_user[userId]) / 2);
        }

        if (pred > 5)
            pred = 5;
        if (pred < 0)
            pred = 0;
    }
    return pred;
```

We are doing some basic exception handling here. If we have no users that have a correlation coefficient higher than 0.4, we make our prediction by getting the mean of our user's mean rating and our movie's mean rating.

If we have a prediction that is not in the range, which can happen due to how we calculate our result, we cap them to the range.

And lastly, we return our prediction.