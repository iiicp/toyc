i = 0;
j = 0;
while (i < 100) {
  j = i + j;
  i = i + 1;
}
k = 0;
do {
j = j - 1;
k = k + 1;
}while (j>0);

for (;k>0;k=k-1)
  j = j + 1;
j;