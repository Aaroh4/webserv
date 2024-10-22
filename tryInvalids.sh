# Color codes
R="\033[0;31m" # Red
G="\033[0;32m" # Green
Y="\033[0;33m" # yellow
B="\033[0;34m" # Blue
P="\033[0;35m" # Purple
C="\033[0;36m" # Cyan

RB="\033[1;31m" # Bold Red
GB="\033[1;32m" # Bold Green
YB="\033[1;33m" # Bold Yellow
BB="\033[1;34m" # Bold Blue
PB="\033[1;35m" # Bold Purple
CB="\033[1;36m" # Bold Cyan

RC="\033[0m" # Reset Color
FLL="**************"


runInvalidConfigs()
{
	printf "${GB}Testing:${RC} ./webserv $1\n"
	printf "${BB}Output:\n${RC}"
	./webserv "$1"
	printf "================================\n\n"
}
for config in invalid_configs/*; do
	# Check if it's a file (not a directory)
	if [ -f "$config" ]; then
		runInvalidConfigs "${config}"
	fi
done
