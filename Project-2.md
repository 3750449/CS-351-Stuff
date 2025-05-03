# Project Objectives

In this project we'll explore throwing more hardware at a problem.  In this situation, we're going to use multiple CPU cores to accelerate our hash computation problem.

As discussed on the midterm exam, the way the data's stored in the file, and even the hash data itself doesn't lend itself well to using the vector instruction set available to us.  However, that doesn't mean we can't use more CPU cores, and indeed we'll see that there's benefit in doing this.  Along the way, we'll learn a bit about a theoretical limit on how much faster we can make a program called _Amdahl's law_.

The explicit goals of this project are:

* collect and analyze data related to the performance of multithreaded programs
* do some additional timing collection to better understand what a program is doing
* look at some theoretical possibilities in performance, and see how well we do towards that goal
* learn to typeset some mathematics using $\LaTeX$

## Project Deliverables

For this project, you'll deliver:
1. short report stored in your GitHub repository (in a directory named **Project-2**, please).  Most of the deliverables for this project are in the report, although we'll do one little bit of coding.
2. a slightly modified copy of the staring program **hash-04.cpp**, which includes some timing instrumentation.

There are some questions presented throughout these instructions that you should include in your report.  They're prefixed by **Question:** to make then easy to find.

## Project Steps

As usual, create a **Project-2** directory in your Git repository, and copy the project files to your local directory using the command (assuming you're in the **Project-2**) directory `cp ~shreiner/public_html/CS-351/Project-2/* .` (including the dot at the end of the command).  This should copy over a number of files:

* **Makefile** for building your programs
* **hash-04.cpp** which is threaded version of the same program from **Project-1**.  Read the comments to see about what's been changed for this version
* **runTrials.sh** which is a helper script to run a number of trials and collect the data for your report. In fact, this script will export a Markdown table that you directly copy-and-paste into your report.  It also computes the speed-up of each version of the program, so you only really need to run this to get the appropriate data.
* **ScopeTimer.h** which is a file containing a helper C++ class that we'll use to get some more fine-grained application timings.  We'll see more about this in a moment.

To begin, build **hash-04** using the `make` command. The program should build cleanly (using the optimization setting of `-g`, which is fine; we're doing apples-to-apples comparisons here, so the optimization isn't crucial yet.)

Run the **runTrials.sh** script, which should output a Markdown table for each run of **hash-04** using differing numbers of threads and all the associated statistics.  Copy this output into your **README.md** for this directory in your repo.  While the table is nice quantitively, a nice graph really emphasizes a major point of this project.  Using your favorite graph-creating tool (like Microsoft Excel, Python, or crayons and graph paper), make a plot of speed-up (the y-axis) vs. number of threads (on the x-axis), and save it as an image (check that image into your **Project-2** directory as well.)

Including image an image in Markdown is very simple, use the following syntax

```markdown
![Alternate description text](<filename> or <URL>)
```
the `<filename>` can be relative to the Markdown file if they're basically in the same directory tree (i.e., if you name your file **image.png**, you can just use that name in your **README.md**)

**Question:** Notice that there is a maximum speed-up factor, but not necessarily using the most threads.  Make a guess (i.e., write a short paragraph) as to why you think more threads aren't necessary better.  Here's a hint: think about a group of people waiting to go through a turnstile (like at BART or Disney World). Are more people able to go through it just because there are more people?

## Amdahl's Law and maximum speed-up

A crazy computer guy back in 1967 presented an equation that suggested the maximum speed-up a computer program might experience for a given number of threads.  This name was [Gene Amdahl](https://en.wikipedia.org/wiki/Gene_Amdahl), and he made the following observation:

> a program has basically two "collections" of code: **serial** code that can't easily be threaded, and **parallel** code that can be executed beneficially across multiple threads.

If you consider those quantities as percentages, you can describe the time a program takes to execute as

$$ T = sT + pT $$

where $sT$ is the proportion of time for executing _serial_ (i.e., single core) code, and $pT$ as the time for executing _parallel_ (i.e., threaded) code.  Of course $s + p = 1$, but more helpful, $s = 1 - p$, which allows us to rewrite the equation as

$$ T = (1 - p)T + pT $$

While that equation is pretty obvious, the magic comes in with the $pT$ term.  In particular, if you "spread" the parallel computation across $n$ cores and assume "perfect threading" (where there are no delays caused by needing to synchronize parallel threads), we can scale the parallel computation time by dividing by the number of cores, yielding an equation of

$$ T = (1 - p)T + \frac{p}{n}T $$

Of course a little factoring and cancellation gives us

$$ 1 = (1 - p) + \frac{p}{n} $$

which can be rewritten as

$$ \textrm{speed-up factor} = \frac{1}{(1 - p) + \frac{p}{n}} $$

for parallel execution times.

For instance, assume that a half of a program's execution time can be threaded, and assume that we're going to use two cores to accelerate the parallel part.  This means

$$ \begin{array}{c} 
 s = .5 \\
 p = .5 \\
 \end{array}
$$

so our speed-up equation looks like

$$ \begin{array}{rl} 
\textrm{speed-up factor} = & \frac{1}{(1 - .5) + \frac{.5}{2}} \\[2mm]
= & \frac{1}{.5 + .25} \\[2mm]
= & \frac{1}{.75} \\[2mm]
= & 1.33 \\
\end{array}
$$

**Question:** Do you think it's possible to get "perfect scaling" &mdash; meaning that the $(1-p)$ terms is zero?  

## Applying Amdahl's Law on our program

Now, we can determine how long the **serial** parts of our program by timing them.  In our case, this mostly comes down to timing the parts that aren't inside the `std::thread` section.

For this part, make a copy of **hash-04.cpp** to something like **timed.cpp** (the filename we'll use to reference this file in the following instructions) or whatever makes you happy.  We can time sections of our code using the `ScopeTimer` class defined in **ScopeTimer.h**.  This is a small C++ class that records a time when it's created, and then when either the `elapsed()` method, or when the object destructs, it samples time again and reports the elapsed time.

To use this to collect some timings, we'll need to do a few things:

1. `#include "ScopeTimer.h"` in **timed.cpp**
2. By default, `ScopeTimer` will report elapsed time in nanoseconds, which may not be convenient to work with when all of our times will be in seconds.  However, we can configure a type that will have `ScopeTimer` report back in seconds.  After including the header, I recommend creating a type to simplify your work using a C++ `using` statement (like the numerous examples in **hash-04.cpp**).  This will work just fine:

```c++
#include "ScopeTimer.h"
using Timer = ScopeTimer<seconds>;
```

I know that doesn't look like much, but now when you create a `Timer` and have it output values, they'll be in seconds, which will simplify the math.

3. `ScopeTimer` is set up to time how long a scope (i.e., the code between a set of matching curly braces) takes to execute.  However, we need to be a little careful because certain variables need to exist, so let's just create a `Timer` at the start of the program.  

Modify your **timer.cpp** and create a `Timer` immediately after `main()`.  Something like this:

```c++
int main(int argc, char *argv[]) {
    Timer timer{"main program"};
    ...
```

4. We can time the first serial section (which ends right before we start creating threads in our program) by seeing how much time has elapsed.  To do this, we just need to call the `elapsed()` method on `Timer`.  

We should call `elapsed()` immediately before we enter the `for` loop creating threads, or in code

```c++
    --numThreads;
    timer.elapsed();
    for (size_t id = 0; id <= numThreads; ++id) {
```
and we'll get to see how long this serial block takes (hint: it ain't much).

5. We'll include the `for` loop in the parallel section of the program (it's not really parallel, but it's simpler this way).  However, after we're done creating and executing our threads, we enter another serial section writing out the results to our file.  We should time that serial section as well, and here we can use a _scope_ to simplify using our `Timer` class.

After the thread creating loop, we want to make a new code block and create a `Timer` in that block.  In code (starting at line 192 in an unmodified **hash-04.cpp**), modify the code to look like the following:

```c++
    {
        Timer{"results output"};
        
        std::string outpath{argv[0]};
        outpath += ".txt";
        std::ofstream output{outpath};

        for (auto hash : hashes) {
            output << hash << "\n";
        }

        output.close();
    }
```

After those (hopefully simple) changes, save and build **timed**.  When you execute it, you should get some output that looks something like:

```bash
% timed 1
main program 0.011118901 s
results output 0.146432644 s
main program 14.315631324 s
```

With those values, we can compute our value $p$ representing the parallel proportion of the program.  The first two times returned represent the serial execution times, so just sum them up.  Dividing that by the entire execution time will give you the percentage of program execution that's executed serially.

$$ \mathrm{serial} = \frac{0.111 + 0.146}{14.316} = 0.017\ (\mathrm{or\ } 1.7\%) $$

So, that's our $s$, giving us $p = 1 - s = .982$, which says that $98.2\%$ of the program's execution time can be accelerated by adding more threads.

Applying Amdahl's law when using two threads, our predicted speed-up would be:

$$ \textrm{speed-up} = \frac{1}{1 - .982 + \frac{.982}{2}} = 1.965 $$

or the program should go about twice as fast.

## Amdahl's estimate

**Question** For your own timings, compute your expected speed-up for 16 cores.  Here's where we'll do a little $\LaTeX$ to make your report look snazzy, just like this one (all the math in here is done in $\LaTeX$)

Since our equation is basically just some words, an equals sign, and a fraction, it's relatively easy to typeset.  When using $\LaTeX$ in Markdown, you create a "stand-alone equation" by putting the $\LaTeX$ commands inside a pair of `$$`, like this

```latex
$$ speedup = 4 $$
```
 which will render as

 $$ speedup = 4 $$

 To create a fraction in $\LaTeX$, you use the `\frac` command (all $\LaTeX$ commands begin with a `\`), so, for example you can do

 ```latex
 $$ speedup = \frac{1}{2} $$
```

to render the fraction $\frac{1}{2}$.  So, for your report, you can build up the equation using this template

```latex
$$ speedup = \frac{1}{1 - p + \frac{p}{n}} = 3.14$$
```

where $p$ is the fraction of code that can be executed in parallel, $n$ is the number of threads (remember, we decided on 16), and replace $\pi$ (3.14) with your computed value above.

If you want to include a formula or variable "inline" with your text, you just bracket the $\LaTeX$ inside of single `$`.  So, when I write "use 16 for $n$", your Markdown would look like

```Markdown
"use 16 for $n$"
```

Editing your report directly in GitHub's website makes this very simple as there's a **Preview** button that will show you what your Markdown looks like.  Similarly, most IDEs (I use VS Code) have a Markdown preview mode.

One final **Question:** in reviewing the graph of speed-ups to number of threads, note that we get pretty _linear_ (when you plot the dots, they're pretty close to being a line) speed-up.  What's the slope of that line? (Pick two values, like for one and seven threads, and do the rise-over-run thing you learned in Algebra).  Does that linear trend continue as we add more threads?  What do you think causes the curve to "flatten out" when we use large thread counts?
