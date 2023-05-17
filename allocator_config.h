/* External config */

/* Memory allocation size in bytes */
#define POOL_SIZE (1024U)

/* Align fixed blocks on X-byte boundary based on CPU architecture.
 * Set value to 1, 2, 4 or 8. */
#define ALLOC_MEM_ALIGN (1U)

/* May be larger due to alignment policy */
#define DESIRED_BLOCK_SIZE (64U)



/* Internal config */

/* Round num_to_round to the next higher multiple */
#define ALLOC_ROUND_UP(num_to_round, multiple) \
    (((num_to_round + multiple - 1U) / multiple) * multiple)

#define ALLOC_BLOCK_SIZE ALLOC_ROUND_UP(DESIRED_BLOCK_SIZE, ALLOC_MEM_ALIGN)
#define NUM_BLOCKS (POOL_SIZE / ALLOC_BLOCK_SIZE)

/* Config verification */
#if (POOL_SIZE < ALLOC_BLOCK_SIZE)
#   error "POOL_SIZE must be greater than or equal to BLOCK_SIZE"
#endif

