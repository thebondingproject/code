git add git_init.bat
git add commit_and_push.bat
git add Master
git add motor_control
git add Temperature_controller

set /p etichetta=Please provide a short commit message (what changes did you do?): 
git commit -m "%etichetta%"
git push origin master
PAUSE