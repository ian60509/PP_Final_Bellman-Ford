OPENMP_PATH_EX=./openMP
GRAPH_ROOT_PATH=/home/chh/pp/test
PERFORMANCE_FILE_PATH=./performance_OpenMP.txt

current_time=$(date +"%Y-%m-%d %H:%M")
echo "------$current_time--------" >> $PERFORMANCE_FILE_PATH


files=$(ls "$GRAPH_ROOT_PATH")

# 逐一處理檔案名稱
for file in $files; do
    # if [ "$file" != "random_500m.graph" ]; then
        echo "$file" >> $PERFORMANCE_FILE_PATH
        $OPENMP_PATH_EX $GRAPH_ROOT_PATH/$file >> $PERFORMANCE_FILE_PATH
    # fi
done

printf "\n"