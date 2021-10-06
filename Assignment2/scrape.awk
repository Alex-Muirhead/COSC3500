BEGIN { FS = ":" }
/per/ {
    count++
    gsub(/ /, "")
    gsub(/us$/, "")
    printf "%4d,\n", $2
}