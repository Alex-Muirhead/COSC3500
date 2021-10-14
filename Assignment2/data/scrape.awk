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
/per/ {
    gsub(/ /, "")
    gsub(/us$/, "")
    printf "[%5d, %8d, %10d],\n", size, $2, total
}