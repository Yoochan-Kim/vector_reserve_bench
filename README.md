```
Method 1: reserve + insert
Run 1: 587694 μs
Run 2: 588293 μs
Run 3: 588522 μs
Run 4: 589348 μs
Run 5: 589704 μs
Run 6: 593231 μs
Run 7: 590023 μs
Run 8: 591454 μs
Run 9: 590690 μs
Run 10: 590530 μs
Average: 589948 μs

Method 2: resize + std::copy
Run 1: 637870 μs
Run 2: 637678 μs
Run 3: 639377 μs
Run 4: 639773 μs
Run 5: 638192 μs
Run 6: 639583 μs
Run 7: 638960 μs
Run 8: 639554 μs
Run 9: 639397 μs
Run 10: 639095 μs
Average: 638947 μs

Method 3: resize + memcpy
Run 1: 639009 μs
Run 2: 641192 μs
Run 3: 639771 μs
Run 4: 640146 μs
Run 5: 640350 μs
Run 6: 639585 μs
Run 7: 639446 μs
Run 8: 639966 μs
Run 9: 640048 μs
Run 10: 640583 μs
Average: 640009 μs
```

### Performance comparison:
- Method 2 speedup vs Method 1: 0.923313x
- Method 3 speedup vs Method 1: 0.921781x
