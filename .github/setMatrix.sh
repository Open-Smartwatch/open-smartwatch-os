if [ "$1" = "--default-lang" ]; then
    # default_lang
    echo $(X=$(if [ -e include/locales/en-US.h ]; then X=$(ls include/locales/en-US.h); else X=$(ls include/locales/*.h -1 | head -1); fi; echo ${X:16:5});X=($X);jq --compact-output --null-input '$ARGS.positional' --args -- "${X[@]}")
elif [ "$1" = "--default-mod" ]; then
    # default_mod
    echo $(X=$(X=$(cat platformio.ini | grep "defau"); echo "${X:15}");X=($X);jq --compact-output --null-input '$ARGS.positional' --args -- "${X[@]}")
elif [ "$1" = "--get-flag" ]; then
    # get_flag
    echo $(X=$(curl https://api.github.com/repos/Open-smartwatch/open-smartwatch.github.io/contents/docs/resources/firmware.md | jq -r ".content" | base64 --decode | grep -o '^-.*` |'); X=$(echo $X | tr '` |-' ' ');X=($X);jq --compact-output --null-input '$ARGS.positional' --args -- "${X[@]}")
elif [ "$1" = "--get-models" ]; then
    # get_models
    echo $(X=$(cat platformio.ini | grep "\[\env:" | sed -e 's/\[\env://' | tr ']\n' ' ');X=($X);jq --compact-output --null-input '$ARGS.positional' --args -- "${X[@]}")
elif [ "$1" = "--get-languages" ]; then
    # get_languages
    echo $(cd include/locales/; X=$(ls *.h -1 | sed -e 's/\.h$//' | tr '\n' ' ');X=($X);jq --compact-output --null-input '$ARGS.positional' --args -- "${X[@]}")
else
    echo "Try, again ! $(date +%F)"
    exit 1
fi



