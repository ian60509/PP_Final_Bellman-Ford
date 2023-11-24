# PP_Final_Bellman-Ford
平行程式期末作業，平行化bellman-ford 演算法
拉拉拉
## 執行環境:
PP工作站....


## 目錄結構說明:
* src: 程式碼
* data: 測資
* include: 需要引用的函式，例如
* 

## Serial-part
編譯:
```
cd...
make clean
make...
```

執行:
```
./example -a ....
```

## MPI-part
編譯:
```
cd...
make clean
make...
```

執行:
```
./example -a ....
```

## OpenMP-part
編譯:
```
cd...
make clean
make...
```

執行:
```
./example -a ....
```

## CUDA part
編譯:
```
cd...
make clean
make...
```

執行:
```
./example -a ....
```
## 實驗結果

## PR 程序
1. fork
2. clone自己的fork repo
3. 開始胡搞瞎搞 => commit => push上自己的fork repo
4. 發Pull Request 給原repo
5. 原repo管理者(ian60509)同意，處理conflict merge

### 取得最新的「原repo」進度
```
git remote -v
```

將原repo設定為upstream 節點
```
git remote add upstream https://github.com/ian60509/PP_Final_Bellman-Ford/edit/main/README.md
git remote -v
```

```
git fetch upstream
git merge upstream/master

```

