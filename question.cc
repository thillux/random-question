#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <regex>
#include <chrono>
#ifdef __gnu_linux__
	#include <unistd.h>
	#include <sys/time.h>
	#include <termios.h>
	#include <regex.h>
	#include <fcntl.h>
#endif // __gnu_linux__

#ifdef WIN32
	#include <Windows.h>
#endif



// http://stackoverflow.com/questions/1413445/read-a-password-from-stdcin
void SetStdinEcho(bool enable = true)
{
#ifdef WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if( !enable )
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;

    SetConsoleMode(hStdin, mode );

#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if( !enable )
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

void readQuestions(std::ifstream& questionFile, std::vector<std::string>& questions) {
  std::regex testRegex("^//.*", std::regex_constants::extended);
  while(questionFile) {
    std::string line;
    std::getline(questionFile, line);
    if (line.length() > 1 && !std::regex_match(line, testRegex))
      questions.push_back(line);
  }
  std::sort(questions.begin(), questions.end());
}

unsigned int getRnd(void) {
  unsigned int rndNumber = 0;
#ifdef __gnu_linux__
  int urandom = open("/dev/urandom", O_RDONLY);
  assert(urandom);
  read(urandom, &rndNumber, sizeof(unsigned int));
  close(urandom);
#endif
#ifdef WIN32
  HCRYPTPROV hProvider = 0;
  CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
  CryptGenRandom(hProvider, sizeof(unsigned int), reinterpret_cast<BYTE*>(&rndNumber));
  CryptReleaseContext(hProvider, 0);
#endif
  return rndNumber;
}

void usage(void) {
    std::cout << "usage: ./question filename" << std::endl;
}

int main(int argc, char ** argv) {
  if (argc != 2) {
      usage();
      return EXIT_FAILURE;
  }

  std::string filename(argv[1]);
  std::vector<std::string> * questions = new std::vector<std::string>;

  std::ifstream questionFile(filename.c_str());
  if (questionFile.good()) {
      readQuestions(questionFile, *questions);
  }
  else {
      std::cerr << "Can't open given file " << filename << std::endl;
      return EXIT_FAILURE;
  }
  questionFile.close();

  std::vector<std::string> * todo = new std::vector<std::string>(*questions);
  std::vector<std::string> * done = new std::vector<std::string>;

  std::string question;
  timeval * starttime = new timeval;
  timeval * endtime = new timeval;
  while(true) {
    int id = getRnd() % todo->size();
    question = (*todo)[id];
    todo->erase(std::find(todo->begin(), todo->end(), question));
    done->push_back(question);

    std::cout << question << std::endl;
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;
	clock::time_point t0 = clock::now();
	SetStdinEcho(false);
    std::cin.get();
    SetStdinEcho(true);
    clock::time_point t1 = clock::now();
    milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
    std::cout <<  total_ms.count() / 1000.0 << " seconds for answer!" << std::endl;
    if (todo->empty()) {
        std::swap(todo, done);
        std::cout << "### Complete, starting again. ###" << std::endl;
    }
  }

  delete starttime;
  delete endtime;
  delete todo;
  delete done;

  return EXIT_SUCCESS;
}
