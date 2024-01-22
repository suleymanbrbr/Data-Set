# Data-Set

In this project, you will implement probabilistic data structures, i.e., sketches, by using bit operations and polymorphism in combination. in particular, you will implement HyperLogLog, KMinVal and Bloom Filter sketches and test them with varying parameters, i.e., sizes and numbers of hash functions, to efficiently count and query a large number of numbers of strings using low memory.

Imagine you’re dealing with a stream of items. Some of these items are repeated, and all are coming from an extremely large universal set - whose size can be infinite. Your first task is to estimate n, the number of unique items in the stream, i.e., stream cardinality. To illustrate, think about trying to determine how many distinct individuals visit Facebook in a given week, where each person logs in multiple times a day. Here the universal set is all people in the world, the stream is people’s login data (with repetitions), and you are interested in the number of unique users. Section 2 introduces some approaches for this cardinality estimation problem, and Section 3 does the same for another problem; set membership.
Finding the number of unique items is not easy; traditional approaches, such as sorting the items or keeping track of the unique items encountered, are not feasible due to their excessive computational demands or memory requirements. Note that you do not know the number of possible items, i.e., the universal set’s cardinality, and even if you know it, it is extremely large. To tackle this challenge, you’ll implement a series of data structures and algorithms that estimates the correct result with much less computational demand. Since it is an estimation, it will not be accurate, but it should fall within an acceptable range. To begin, we create a hypothetical stream having n unique items but also repetitive entries by following these steps:
• Generate n random integers from an arbitrary distribution.
• Randomly duplicate some of these numbers a variable number of times.
• Shuffle the resulting dataset.

However, since the universal set cardinality is infinite, and the original numbers are distributed arbitrarily (see the first step above), to ensure that the values we deal with are evenly distributed within a limited interval, we will employ a hash function and estimate the stream cardinality based on the hashed values rather than the actual entries themselves. You can consider a hash function as a deterministic, non-random function generating uniformly distributed numbers. It is deterministic since for the same input, which can be an integer, double, string, any object etc., a hash function outputs the same number. In the project, the items will be words/strings. A basic hash function family implementation, which you can also use in your project, is given below:

<img width="727" alt="Screenshot 2024-01-22 at 14 55 08" src="https://github.com/suleymanbrbr/Data-Set/assets/111366311/84f03d9c-f486-4c57-84a2-37cfc89029da">






<img width="611" alt="Screenshot 2024-01-22 at 14 57 19" src="https://github.com/suleymanbrbr/Data-Set/assets/111366311/48b4a6c9-1f32-4b37-9ede-080b053418a7">











<img width="325" alt="Screenshot 2024-01-22 at 14 57 43" src="https://github.com/suleymanbrbr/Data-Set/assets/111366311/65b98fb6-6c91-447d-a3c2-62f432cdb820">








<img width="482" alt="Screenshot 2024-01-22 at 14 57 52" src="https://github.com/suleymanbrbr/Data-Set/assets/111366311/02659d32-0634-40cf-961f-d8d35a2860f7">

