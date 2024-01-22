// Süleyman Berber 31293

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <climits>
#include <algorithm>
#include <numeric>
#include <cstdint>

using namespace std;

// Constants for MurmurHash
constexpr uint64_t c1 = 0xcc9e2d51ull;
constexpr uint64_t c2 = 0x1b873593ull;
constexpr uint64_t r1 = 15;
constexpr uint64_t r2 = 13;
constexpr uint64_t m = 5;
constexpr uint64_t n = 0xe6546b64ull;

// Constants for hash functions ax+b
constexpr uint64_t a_values[5] = { 97102761753631939, 56842397421741207, 61811894731001917, 31415926535897931, 98765432109876543 };
constexpr uint64_t b_values[5] = { 42506983374872291, 18456721873196387, 37217717671130671, 27182818284590453, 57548533468232177 };

// MurmurHash function
uint64_t murmur_hash(const std::string& key, uint64_t seed = 0) {
  uint64_t hash = seed;
  const uint8_t* data = reinterpret_cast<const uint8_t*>(key.c_str());
  size_t len = key.length();
  for (size_t i = 0; i < len / 8; ++i) {
    uint64_t k = reinterpret_cast<const uint64_t>(data + i * 8);
    k *= c1;
    k = (k << r1) | (k >> (64 - r1));
    k *= c2;

    hash ^= k;
    hash = ((hash << r2) | (hash >> (64 - r2))) * m + n;
  }
  const uint8_t* tail = data + (len / 8) * 8;
  uint64_t k1 = 0;
  switch (len & 7) {
    case 7:
      k1 ^= static_cast<uint64_t>(tail[6]) << 48;
    case 6:
      k1 ^= static_cast<uint64_t>(tail[5]) << 40;
    case 5:
      k1 ^= static_cast<uint64_t>(tail[4]) << 32;
    case 4:
      k1 ^= static_cast<uint64_t>(tail[3]) << 24;
    case 3:
      k1 ^= static_cast<uint64_t>(tail[2]) << 16;
    case 2:
      k1 ^= static_cast<uint64_t>(tail[1]) << 8;
    case 1:
      k1 ^= static_cast<uint64_t>(tail[0]);
      k1 *= c1;
      k1 = (k1 << r1) | (k1 >> (64 - r1));
      k1 *= c2;
      hash ^= k1;
  }
  hash ^= len;
  hash ^= (hash >> 37);
  hash *= 0x85ebca6bull;
  hash ^= (hash >> 32);
  return hash;
}

// Hash function ax+b for a single string
uint64_t ax_b_string_func(uint64_t a, uint64_t b, string to_hash) {
  uint64_t hash = b;
  for (char c : to_hash) {
    hash = hash * a + static_cast<uint64_t>(c);
  }
  return (uint64_t)hash;
}

// Abstract class for sketching
class Sketch {
public:
  virtual void add(string s) = 0; // Abstract method for adding elements to the sketch
};

// Abstract class for membership sketching
class MembershipSketch : Sketch {
public:
  virtual bool isMember(string s) = 0; // Abstract method for checking membership in the sketch
};

// BloomFilter class for membership sketching
class BloomFilter : MembershipSketch {
public:
  // Constructor for BloomFilter
  BloomFilter(int k, int size, int _switch)
    : k(k), size(size), _switch(_switch), filter(new uint64_t[size / 64]) {
    for (int i = 0; i < size / 64; i++) {
      filter[i] = 0;
    }
  }

  // Destructor for BloomFilter
  ~BloomFilter() {
    delete[] filter;
  }

  // Set hash values for a given word
  void setHashes(string word) {
    if (_switch == 1) {
      hash1 = ax_b_string_func(a_values[0], b_values[0], word) % size;
      hash2 = ax_b_string_func(a_values[1], b_values[1], word) % size;
      hash3 = ax_b_string_func(a_values[2], b_values[2], word) % size;
      hash4 = ax_b_string_func(a_values[3], b_values[3], word) % size;
      hash5 = ax_b_string_func(a_values[4], b_values[4], word) % size;
    } else {
      hash1 = murmur_hash(word, a_values[0]) % size;
      hash2 = murmur_hash(word, a_values[1]) % size;
      hash3 = murmur_hash(word, a_values[2]) % size;
      hash4 = murmur_hash(word, a_values[3]) % size;
      hash5 = murmur_hash(word, a_values[4]) % size;
    }
  }

  // Add a word to the BloomFilter
  void add(string word) {
    setHashes(word);

    filter[hash1 / 64] |= (temp << (63 - (hash1 % 64)));
    filter[hash2 / 64] |= (temp << (63 - (hash2 % 64)));
    filter[hash3 / 64] |= (temp << (63 - (hash3 % 64)));
    filter[hash4 / 64] |= (temp << (63 - (hash4 % 64)));
    filter[hash5 / 64] |= (temp << (63 - (hash5 % 64)));
  }

  // Check if a word is a member of the BloomFilter
  bool isMember(string word) {
    setHashes(word);

    return (filter[hash1 / 64] & (temp << (63 - (hash1 % 64)))) > 0 &&
           (filter[hash2 / 64] & (temp << (63 - (hash2 % 64)))) > 0 &&
           (filter[hash3 / 64] & (temp << (63 - (hash3 % 64)))) > 0 &&
           (filter[hash4 / 64] & (temp << (63 - (hash4 % 64)))) > 0 &&
           (filter[hash5 / 64] & (temp << (63 - (hash5 % 64)))) > 0;
  }

  // Getter for size of the filter
  int get_size() {
    return size;
  }

  // Getter for the number of hash functions
  int get_k() {
    return k;
  }

private:
  uint64_t hash1, hash2, hash3, hash4, hash5, temp = 1;
  int k;              // number of hash functions
  int size;           // size of array
  int _switch;        // switch to select the hash function
  uint64_t* filter;   // BloomFilter array
};

// Abstract class for cardinality sketching
class CardinalitySketch : Sketch {
public:
  virtual double estimateCardinality() = 0; // Abstract method for estimating cardinality
};

// HyperLogLog class for cardinality sketching
class HyperLogLog : CardinalitySketch {
public:
  // Constructor for HyperLogLog
  HyperLogLog(int b, int _switch)
    : b(b), _switch(_switch), buckets(new uint64_t[b]) {
    for (int i = 0; i < b; i++)
      buckets[i] = 0;
  }

  // Destructor for HyperLogLog
  ~HyperLogLog() {
    delete[] buckets;
  }

  // Add a word to HyperLogLog
  void add(string word) {
    uint64_t hash, consecutive_number = 0;

    if (_switch == 1)
      hash = ax_b_string_func(a_values[4], b_values[4], word);
    else
      hash = murmur_hash(word, c2);

    uint64_t updated = hash >> (64 - static_cast<int>(log2(b)));

    while ((hash & 1) == 0) {
      consecutive_number++;
      hash = hash >> 1;
    }

    if (buckets[updated] < (consecutive_number + 1))
      buckets[updated] = consecutive_number + 1;
  }

  // Estimate the cardinality using HyperLogLog algorithm
  double estimateCardinality() {
    double a_m = 0;
    if (b <= 16)
      a_m = 0.673;
    else if (b <= 32)
      a_m = 0.697;
    else if (b <= 64)
      a_m = 0.709;
    else if (b >= 128)
      a_m = 0.7213 / (1 + 1.079 / b);

    double sum = 0;
    for (int i = 1; i <= b; i++) {
      double result = std::pow(0.5, buckets[i]);
      sum += result;
    }

    return a_m * b * (b / sum);
  }

private:
  int b;           // number of buckets
  uint64_t* buckets; // HyperLogLog array
  int _switch;      // switch to select the hash function
};

// KMinVal class for cardinality sketching
class KMinVal : CardinalitySketch {
public:
  // Constructor for KMinVal
  KMinVal(int K, int _switch)
    : K(K), val(new uint64_t[K]), _switch(_switch) {
    for (int i = 0; i < K; i++) {
      val[i] = UINT64_MAX;
    }
  }

  // Destructor for KMinVal
  ~KMinVal() {
    delete[] val;
  }

  // Add a word to KMinVal
  void add(string word) {
    uint64_t temp;

    if (_switch == 1)
      temp = ax_b_string_func(a_values[3], b_values[3], word);
    else
      temp = murmur_hash(word, a_values[3]);

    if (val[K - 1] > temp) {
      val[K - 1] = temp;
    }
    std::sort(val, val + K);
  }

  // Estimate the cardinality using KMinVal algorithm
  double estimateCardinality() {
    double result = ((K - 1) * (UINT64_MAX / (val[K - 1] * 1.0)));
    return result;
  }

private:
  int K;          // number of minimum values to keep
  uint64_t* val; // KMinVal array
  int _switch;    // switch to select the hash function
};

// Function to evaluate BloomFilter
void evaluate_bloom(vector<string> words, BloomFilter* bloom, int i) {
  int true_positives = 0;
  int false_positives = 0;

  for (int i = 0; i < 100; i++) {
    true_positives += (int)bloom->isMember(words[i]);
  }

  for (int i = 0; i < 100; i++) {
    false_positives += (int)bloom->isMember(words[words.size() - i - 1]);
  }

  cout << "size: " << bloom->get_size() << " k: " << bloom->get_k() << " Inserted: " << i << " TP: " << true_positives << " FP: " << false_positives << endl;
}

int main(int argc, char** argv){

  string fname;
  fname = argv[1];
  
  ifstream file(fname);
  if(!file.is_open()){
    cout << "Error opening file: " << fname << endl;
    return 1;
  }

  vector<string> words;
  string word;

  //Reading each line into vector of strings
  while(std::getline(file, word)){
    words.push_back(word);
  }

  cout << "#######################################################################################" << endl;
  cout << "HyperLogLog Using ax+b hash" << endl;
  for(int an = 2; an <= 8192; an *= 2){
    HyperLogLog loglog(an, 1);
    for(int i = 0; i < words.size(); i++){
      loglog.add(words[i]);
    }
    
    cout << "buckets: " << an << " HyperLogLog estimate: " << loglog.estimateCardinality() << endl;
  }
  cout << "#######################################################################################" << endl;

  cout << "#######################################################################################" << endl;
  cout << "HyperLogLog Using MurMurHash" << endl;
  for(int an = 2; an <= 8192; an *= 2){
    HyperLogLog loglog(an, 2);
    for(int i = 0; i < words.size(); i++){
      loglog.add(words[i]);
    }
    
    cout << "buckets: " << an << " HyperLogLog estimate: " << loglog.estimateCardinality() << endl;
  }
  cout << "#######################################################################################" << endl;

  cout << "#######################################################################################" << endl;
  cout << "KMinVal Using ax + b hash" << endl;
  for(int an = 2; an <= 8192; an *= 2){
    KMinVal kminval(an, 1);
    for(int i = 0; i < words.size(); i++){
      kminval.add(words[i]);
    }
    cout << "buckets: " << an << " KMinVal estimate: " << kminval.estimateCardinality() << endl;
  }
  cout << "#######################################################################################" << endl;

  cout << "#######################################################################################" << endl;
  cout << "KMinVal Using MurMurHash" << endl;
  for(int an = 2; an <= 8192; an *= 2){
    KMinVal kminval(an, 2);
    for(int i = 0; i < words.size(); i++){
      kminval.add(words[i]);
    }
    cout << "buckets: " << an << " KMinVal estimate: " << kminval.estimateCardinality() << endl;
  }
  cout << "#######################################################################################" << endl;

  cout << "#######################################################################################" << endl;
  cout << "BloomFilter using ax + b hash" << endl;
  for(int i = 64; i <= 131072; i *= 2){
    BloomFilter *bloom = new BloomFilter(5, i, 1); //Note that we are using 5 hashes with ax + b
    for(int i = 0; i < words.size() - 100; i++){ //We do not add last 100 words to the bloom filter to be able to check if they return as false positives
      bloom->add(words[i]);
      if(i == 100)
    evaluate_bloom(words, bloom, i);
      if(i == 500)
    evaluate_bloom(words, bloom, i);
      if(i == 1000)
    evaluate_bloom(words, bloom, i);
      if(i == 100000)
    evaluate_bloom(words, bloom, i);
      if(i == 1000000)
    evaluate_bloom(words, bloom, i);
    }
    delete bloom;
  }
  cout << "#######################################################################################" << endl;

  cout << "#######################################################################################" << endl;
  cout << "BloomFilter using MurMurHash" << endl;
  for(int i = 64; i <= 131072; i *= 2){
    BloomFilter *bloom = new BloomFilter(5, i, 2); //Note that we are using 5 hashes with ax + b
    for(int i = 0; i < words.size() - 100; i++){ //We do not add last 100 words to the bloom filter to be able to check if they return as false positives
      bloom->add(words[i]);
      if(i == 100)
    evaluate_bloom(words, bloom, i);
      if(i == 500)
    evaluate_bloom(words, bloom, i);
      if(i == 1000)
    evaluate_bloom(words, bloom, i);
      if(i == 100000)
    evaluate_bloom(words, bloom, i);
      if(i == 1000000)
    evaluate_bloom(words, bloom, i);
    }
    delete bloom;
  }
  cout << "#######################################################################################" << endl;

    
}
