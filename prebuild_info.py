import subprocess
Import("env")

# Try to execute Git, if it failes we will just display the defaults below
gitAvailable = False
gitCommitHash = "???"
gitCommitTime = "???"
try:
    proc = subprocess.Popen(['git', '--version'], stdout=subprocess.PIPE)
    proc.communicate()
    gitAvailable = proc.returncode == 0
except:
    pass

if gitAvailable:
    gitCommitHash = subprocess.Popen(
        ['git', 'rev-parse', 'HEAD'], stdout=subprocess.PIPE).communicate()[0].decode('utf-8').strip()
    gitCommitTime = subprocess.Popen(
        ['git', 'log', '-1', '--pretty=format:%ai'], stdout=subprocess.PIPE).communicate()[0].decode('utf-8').strip()

env.Append(CPPDEFINES=[
    # This will be expanded into a command line argument like '-DGIT_COMMIT_HASH="\"str\""', and thats the reason for the "strange" backslashing...
    ("GIT_COMMIT_HASH", "\\\"" + gitCommitHash + "\\\""),
    ("GIT_COMMIT_TIME", "\\\"" + gitCommitTime + "\\\"")
])
