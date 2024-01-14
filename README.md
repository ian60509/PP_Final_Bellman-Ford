# PP_Final_Bellman-Ford
平行程式期末作業，平行化bellman-ford 演算法

## 執行環境:
PP工作站....


## 目錄結構說明:
* src: 程式碼
* data: 測資
* include: 需要引用的函式
* common: helper function
* tools: 生圖工具
* result: 實驗結果

## Serial-part
編譯 + 執行:
```
cd /src/Serial
make clean
make run
```


## MPI-part
編譯+執行:
```
cd ./src/MPI
run.sh
```


## OpenMP-part
編譯+執行:
```
cd ./src/MPI
./shell.sh
```

## CUDA part
編譯:
```
cd /src/Serial
make clean
make
```

執行:
```
./bellman_ford_cuda <graph path>
=======
cd ./src/CUDA
make clean
make all

```




