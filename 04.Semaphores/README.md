# Semaphores
Program that generates a series of producers and consumers processes. The first ones will create products (ints in this case)
in a certain number of critical sections, sections that only can contain one product per time. This product must be consumed by a consumer to make available
that critical section again. This cycle continues until a certain number of products are produced.
