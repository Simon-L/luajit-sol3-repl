#! /bin/bash

input="$(< /dev/stdin)"
inputbase64="$(echo "${input}" | base64)"
curl --request POST http://localhost:8888/api/eval \
  -d "{\"code\":\"$inputbase64\"}" \
  | jq

exit 0
