<<<<<<< HEAD
# CS-351-Stuff



| Program   | Optimization | Real Time (s) | User Time (s) | Sys Time (s) | Memory (KB) | Throughput (MB/s) | Speedup vs hash-00 -g |
|-----------|--------------|---------------|----------------|---------------|--------------|--------------------|------------------------|
| hash-00   | -g           | 350.20        | 343.79         | 4.49          | 2880         | 0.05               | 1.00                   |
| hash-01   | -g           | 18.08         | 16.64          | 1.34          | 2892         | 0.88               | 19.37                  |
| hash-02   | -g           | 15.31         | 14.17          | 1.03          | 2892         | 1.05               | 22.88                  |
| hash-03   | -g           | 16.19         | 15.09          | 0.98          | 2880         | 0.99               | 21.63                  |
| hash-04   | -g           | 14.15         | 13.55          | 0.51          | 5011784      | 1.13               | 24.75                  |
| hash-00   | -O2          | 335.48        | 329.86         | 3.67          | 2888         | 0.05               | 1.04                   |
| hash-01   | -O2          | 8.19          | 6.79           | 1.31          | 2880         | 1.95               | 42.75                  |
| hash-02   | -O2          | 8.65          | 6.84           | 1.32          | 2888         | 1.85               | 40.47                  |
| hash-03   | -O2          | 8.20          | 6.83           | 1.24          | 2880         | 1.95               | 42.69                  |
| hash-04   | -O2          | 7.41          | 6.67           | 0.54          | 5011784      | 2.16               | 47.25                  |

=======
|Thread<br>Count|Wall Clock<br>Time|User Time|System Time|Speedup|
|:--:|--:|--:|--:|:--:|
|1|16.82|13.95| 0.79|1.00|
|2| 8.03|14.88| 0.64| 2.09|
|3| 5.80|15.60| 0.69| 2.90|
|4| 4.69|16.33| 0.79| 3.59|
|5| 3.80|15.60| 1.04| 4.43|
|6| 3.30|15.90| 1.03| 5.10|
|7| 3.04|16.20| 1.30| 5.53|
|8| 2.83|16.63| 1.38| 5.94|
|16| 2.14|17.74| 3.08| 7.86|
|24| 1.92|18.25| 5.53| 8.76|
|32| 1.93|17.88|12.45| 8.72|
|40| 1.95|17.28|16.57| 8.63|
|48| 1.99|17.11|15.68| 8.45|
|56| 1.93|16.81|21.93| 8.72|
|64| 1.97|16.66|21.46| 8.54|
|72| 2.00|17.08|15.01| 8.41|
|80| 1.97|16.94|20.93| 8.54|

---

## Speed-Up Graph

![Speed-up graph](speedup.png)

---

## Amdahl’s Law Prediction

From timing:

- Serial before threads: 0.0090 s
- Results output: 0.1319 s
- Total time: 4.1904 s

Total serial time:
$$
s = 0.0090 + 0.1319 = 0.1409
$$

Parallel fraction:
$$
p = 1 - \frac{0.1409}{4.1904} \approx 0.9664
$$

Predicted speed-up with 16 threads:
$$
\text{speedup} = \frac{1}{1 - 0.9664 + \frac{0.9664}{16}} = \frac{1}{0.0336 + 0.0604} = \frac{1}{0.094} \approx 10.64
$$

---

## Discussion Questions

### Why isn't more always better?

After a certain number of threads, 
overhead from thread management and synchronization outweighs gains. 
It's like trying to squeeze more people through 
a single door—eventually, it doesn't help.

### Is perfect scaling possible?

No. 
Some code must always run sequentially 
(e.g., setup, output), and thread coordination adds overhead. 
So, there's always a limit.

### What's the slope of the speed-up curve?

Using 1 thread = 1.00 and 8 threads = 5.94:

$$
\text{slope} = \frac{5.94 - 1.00}{8 - 1} = \frac{4.94}{7} \approx 0.71
$$

The trend flattens beyond 8–16 threads 
as parallel efficiency drops due to I/O, locking, 
and limited available parallel work.


>>>>>>> fde2c7f (Initial commit for Project 2)
