# 架構
1. 使用ProduceText，輸入要求vertice數量，edge 數量, sparse, dense, ..... => 生成目標TextFile
2. 使用 graphTools 將TextFile 轉換成 BinaryFile
3. BinaryFile 可以用來當作我們實驗的輸入檔案

# 使用我製作的工具ProduceText生成Text File
#### 編譯 
1. make clean
2. make PT

#### 執行
```
./produce_text 
```


# 使用 graphTools
## 執行檔案
1. 先製作好text file
2. make
3. ./graphTools => 會跳出錯誤通知訊息，提示該使用哪些參數，以及提供檔案的類型，
4. 選擇想要的功能(注意需求檔案是text, binary )
### option
"text2bin": 將文字檔轉為二進位檔案

"info" : 輸出基本的Metadata，點、邊的數量

"print" : 輸出圖上每個點的incoming, outgoing edge

"noout": 列出所有沒有outgoing edge 的點

"noin": 列出所有沒有incoming edge 的點

"edgestats": 列出和edge有關的統計數值

## TroubleShoot
注意檔案是text or  binary ，如果錯誤可能跳出
Invalid graph file header. File may be corrupt.