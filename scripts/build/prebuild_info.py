import subprocess
Import("env")

# Try to execute Git, if it failes we will just display the defaults below
gitAvailable = False
gitCommitHash = "???"
gitCommitTime = "???"
gitBranchName = "???"
try:
    proc = subprocess.Popen(['git', '--version'], stdout=subprocess.PIPE)
    proc.communicate()
    gitAvailable = proc.returncode == 0
except:
    pass

if gitAvailable:
    gitCommitHash = subprocess.Popen(
        ['git', 'rev-parse', 'HEAD'], stdout=subprocess.PIPE).communicate()[0].decode('utf-8').strip()[:7] # Short hash
    gitCommitTime = subprocess.Popen(
        ['git', 'log', '-1', '--pretty=format:%cd', '--date=format:%Y-%m-%dT%H:%M:%S%z'], stdout=subprocess.PIPE).communicate()[0].decode('utf-8').strip()
    gitBranchName = subprocess.Popen(
        ['git', 'rev-parse', '--abbrev-ref', 'HEAD'], stdout=subprocess.PIPE).communicate()[0].decode('utf-8').strip()

env.Append(CPPDEFINES=[
    # This will be expanded into a command line argument like '-DGIT_COMMIT_HASH="\"str\""', and thats the reason for the "strange" backslashing...
    ("GIT_COMMIT_HASH", "\\\"" + gitCommitHash + "\\\""),
    ("GIT_COMMIT_TIME", "\\\"" + gitCommitTime + "\\\""),
    ("GIT_BRANCH_NAME", "\\\"" + gitBranchName + "\\\""),
    ("PIO_ENV_NAME", "\\\"" + env['PIOENV'] + "\\\"")
])
