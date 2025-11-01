# Demo14_2_SD_FATFS_DMA

# create a new repository on the command line
echo "# Demo14_2_SD_FATFS_DMA" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin https://github.com/wenchm/Demo14_2_SD_FATFS_DMA.git
git push -u origin main

# push an existing repository from the command line
git remote add origin https://github.com/wenchm/Demo14_2_SD_FATFS_DMA.git
git branch -M main

# add all files
git status
git add .
git commit -m "origin"
git push -u origin main

Enumerating objects: 368, done.
Counting objects: 100% (368/368), done.
Delta compression using up to 8 threads
Compressing objects: 100% (346/346), done.
Writing objects: 100% (367/367), 7.58 MiB | 2.97 MiB/s, done.
Total 367 (delta 128), reused 0 (delta 0), pack-reused 0 (from 0)
remote: Resolving deltas: 100% (128/128), done.
To https://github.com/wenchm/Demo14_2_SD_FATFS_DMA.git
   4d8f590..d04b9ce  main -> main

# 已经关闭CRLF自动转换和gitignore。
