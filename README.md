A friend of mine, a computer scientist working on memory consistency models theory in C and C++, has relayed to me an extraordinary claim, which boils down to this: "using memory barriers in code for each read and write does not have an impact on performance." Now, this claim is something he's heard second hand, and since he is an extreme example of a theory-oriented guy, the validity of this claim was not something he was going to check.

To me, wearing a hat of a systems programmer (apparently we're a dwindling population), this seemed ridiculous as it reduces to "we don't need CPU caches". I've written code where performance died from over-use of protection mechanism, and in writing this article I've repeatedly encountered similar experiences ([like this](http://bartoszmilewski.com/2008/11/05/who-ordered-memory-fences-on-an-x86/)). This should be a no-brainer. So, after a brief argument, which actually become about did he even hear the original statement correctly, I said I'll check it myself and report the results. Because, like, I don't have better things to do, apparently.

So, here it is.

## The environment

I'm doing this with GCC on Windows, and I'm using what C++11 provides, without external libraries such as Boost. First, let's see what the std::atomic template type does.

A plain store (`int x`) looks like this:

	movl	$666, 8(%esp)

A store with atomic types (`std::atomic<int> y`) looks like this:

	movl	$777, %eax
	xchgl	12(%esp), %eax

So, it uses `xchgl`, which is [documented](http://www.fermimn.gov.it/linux/quarta/x86/xchg.htm) to behave "if a memory operand is involved, BUS LOCK is asserted for the duration of the exchange". A bus lock, [Intel says](http://www.cs.cmu.edu/~410-f10/doc/Intel_Reordering_318147.pdf) in the context of "Intel 64 memory ordering", means "In a multiprocessor system, locked instructions have a total order," where the wording "total order" means the pipelines are blown and caches are committed immediately. It acts as a memory fence.

Now, this is on x86, so the compiler will not do anything special for reads, but that's ok. I think this is a fair test of the theory at hand. So, I'm going to find some code and test how it behaves with the two variants.

## The benchmark

I've chosen square matrix multiplication code with 1 MB matrices (i.e. 3 of them are involved in the operation). The main part of the algorithm is this loop:

	for (c = 0; c < m; c++) {
	  for (d = 0; d < q; d++) {
		for (k = 0; k < p; k++) {
		  sum = sum + first[c][k]*second[k][d];
		}
		multiply[c][d] = sum;
		sum = 0;
	  }
	}

All the variables here are `int`'s, and I will compile the code once, regularly, and once with `int` re-`define`d as `std::atomic<int>` (yay C preprocessor shenanigans!).

## The result

I'm running the test on my [Broadwell-U](http://ark.intel.com/products/85212/Intel-Core-i5-5200U-Processor-3M-Cache-up-to-2_70-GHz) laptop. I've taken into account CPU frequency scaling by having a warm-up run of 10 seconds so the frequency has enough time to ramp up.

**The "plain int" version took 0.33 seconds for a single multiplication.**

**The overly-memory-fenced version took 5.67 seconds.**

This is about 17.18 times slower than the "plain" version!

To be honest, I've expected it to be two orders of magnitude slower, so yeah, congratulations to Intel's CPU engineers!
