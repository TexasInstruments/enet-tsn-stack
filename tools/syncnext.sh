#!/bin/sh

set -e
trap '[ $? -eq 0 ] && exit 0 || echo "$0 FAILED at line ${LINENO}"' EXIT

WORK_TEMP_DIR=/tmp/tsn_stack_sync_workdir
DIFF_FILE=/tmp/diff_file.txt

git fetch origin next
git fetch origin internal
git checkout origin/internal
REF_COMMIT_ID=`git rev-parse origin/internal`

# Generate output files
rm -rf $WORK_TEMP_DIR
mkdir -p $WORK_TEMP_DIR
./tools/release.sh $WORK_TEMP_DIR
# create missing filename list
diff -qr $WORK_TEMP_DIR . | sed '/tools/d' | sed '/.git/d' | sed '/internal_docs/d' > $DIFF_FILE

echo "internal_docs/" >> $WORK_TEMP_DIR/.gitignore
echo "tools/" >> $WORK_TEMP_DIR/.gitignore
awk '/^Only/  { sub(/:$/,        "", $3     ); print $3"/"$4     }' $DIFF_FILE | sort -u | sed 's/^.\{,2\}//' >> $WORK_TEMP_DIR/.gitignore

git checkout origin/next

COMMIT_MSG="allsocs: tsn: Pull from internal branch\n\nRef-commit-id:"$REF_COMMIT_ID
rm -rf *
cp -Tr $WORK_TEMP_DIR/ .
git add .
git commit -m "allsocs: tsn: Pull from internal branch" -m "Ref-commit-id: "$REF_COMMIT_ID
git checkout -b tsn_internal_2_next_sync
#git push origin tsn_internal_2_next_sync
git rev-parse HEAD
rm -rf $WORK_TEMP_DIR

echo "################ SUCCESS #######################"
