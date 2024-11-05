#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <utility>

using namespace std;

const int NUM_BOOKS = 64;
const int TOP_WORDS = 100;
const set<string> EXCLUDED_WORDS = {"a", "and", "an", "of", "in", "the"};

struct BookData {
    string filename;
    unordered_map<string, double> normalized_frequencies;
    double total_words = 0.0;
};

void preprocessBook(BookData &book) {
    ifstream file(book.filename);
    string word;
    unordered_map<string, int> word_count;

    while (file >> word) {
        
        transform(word.begin(), word.end(), word.begin(), ::toupper);
    
        word.erase(remove_if(word.begin(), word.end(), [](char c) {
            return !isalnum(c);
        }), word.end());

        if (EXCLUDED_WORDS.find(word) == EXCLUDED_WORDS.end() && !word.empty()) {
            word_count[word]++;
            book.total_words++;
        }
    }

    vector<pair<string, double>> freq_vector(word_count.begin(), word_count.end());
    sort(freq_vector.begin(), freq_vector.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });

    for (int i = 0; i < min(TOP_WORDS, static_cast<int>(freq_vector.size())); ++i) {
        book.normalized_frequencies[freq_vector[i].first] = freq_vector[i].second / book.total_words;
    }
}

double calculateSimilarity(const BookData &book1, const BookData &book2) {
    double similarity = 0.0;
    for (const auto &pair : book1.normalized_frequencies) {
        const string &word = pair.first;
        if (book2.normalized_frequencies.find(word) != book2.normalized_frequencies.end()) {
            similarity += pair.second * book2.normalized_frequencies.at(word);
        }
    }
    return similarity;
}

vector<pair<string, string>> findSimilarPairs(const vector<BookData> &books) {
    vector<pair<string, string>> similar_pairs;
    vector<pair<double, pair<int, int>>> similarities;

    for (int i = 0; i < NUM_BOOKS; ++i) {
        for (int j = i + 1; j < NUM_BOOKS; ++j) {
            double similarity = calculateSimilarity(books[i], books[j]);
            similarities.push_back({similarity, {i, j}});
        }
    }

    sort(similarities.begin(), similarities.end(), greater<>());

    for (int i = 0; i < 10 && i < similarities.size(); ++i) {
        int index1 = similarities[i].second.first;
        int index2 = similarities[i].second.second;
        similar_pairs.push_back({books[index1].filename, books[index2].filename});
    }

    return similar_pairs;
}

int main() {
    vector<BookData> books(NUM_BOOKS);

    for (int i = 0; i < NUM_BOOKS; ++i) {
        books[i].filename = "book"[i] + ".txt";
        preprocessBook(books[i]);
    }

    auto similar_pairs = findSimilarPairs(books);

    cout << "Top 10 Similar Pairs of Textbooks:\n";
    for (const auto &pair : similar_pairs) {
        cout << pair.first << " <-> " << pair.second << "\n";
    }

    return 0;
}