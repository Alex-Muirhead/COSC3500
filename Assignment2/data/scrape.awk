BEGIN { FS = ":" }
/size/ {
    gsub(/ /, "")
    size = $2
}
/per/ {
    count++
    gsub(/ /, "")
    gsub(/us$/, "")
    printf "[%5d, %5d],\n", size, $2
}