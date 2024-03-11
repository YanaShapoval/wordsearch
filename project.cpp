#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <sstream>

using namespace std;

// Application function to search for words in the text
void application(const string& text, const string& word, long allowed_delay, int num_iterations, long& total_search_time, int& total_word_count) {
    auto start = chrono::steady_clock::now();
    //for (int iter = 0; iter < num_iterations; ++iter) {
        size_t pos = text.find(word);
        int count = 0;
        while (pos != string::npos) {
            count++;
            pos = text.find(word, pos + 1);
        }
        total_word_count += count;
    //}
    auto end = chrono::steady_clock::now();
    total_search_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

// Controller function to manage the application based on the time taken
void controller(const string& text, const string& word, long allowed_delay, int num_threads, int num_iterations, long& total_search_time, int& total_word_count) {
    // Divide the text into threads based on lines
    vector<thread> threads;
    istringstream iss(text);
    vector<string> lines;
    string line;
    while (getline(iss, line)) {
        lines.push_back(line);
    }
    size_t lines_per_thread = lines.size() / num_threads;
    size_t start = 0;
    for (int i = 0; i < num_threads; ++i) {
        size_t end = min(start + lines_per_thread + 1, lines.size());
        string thread_text;
        for (size_t j = start; j < end; ++j) {
            thread_text += lines[j] + "\n";
        }
        threads.emplace_back([thread_text, word, allowed_delay, num_iterations, &total_search_time, &total_word_count]() {
            application(thread_text, word, allowed_delay, num_iterations, total_search_time, total_word_count);
        });
        start = end;
    }
    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    // Read the text from the file
    ifstream file("random.txt");
    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    // Parameters
    string word = "how"; // Word to search for
    long allowed_delay = 1000; // Allowed delay in milliseconds
    int num_threads = 1; // Initial number of threads
    int num_iterations = 200000; // Number of iterations

    long total_search_time = 0; // Total search time for all iterations
    int total_word_count = 0; // Total number of words found for all iterations

    // Execute the application multiple times
    controller(text, word, allowed_delay, num_threads, num_iterations, total_search_time, total_word_count);

    // Print out the total search time and the total number of words found
    cout << "Total search time for all iterations: " << total_search_time << " milliseconds" << endl;
    cout << "Total number of words found: " << total_word_count << endl;
    cout << "The number of threads: " << num_threads << endl;
    

    return 0;
}
