BEGIN { FS = ":" }
/size/ {
    gsub(/ /, "")
    size = $2
}
/Total time/ {
    gsub(/ /, "")
    gsub(/us$/, "")
    total = $2
}
/multiply function used/ {
    gsub(/ /, "")
    gsub(/us$/, "")
    mult = $2
}
/eigensolver library used/ {
    gsub(/ /, "")
    gsub(/us$/, "")
    eig = $2
    printf "Size: %5d, Percent used: %4.1f%%\n", size, mult/total*100
}