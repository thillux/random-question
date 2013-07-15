#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <memory>

#if defined __gnu_linux__ || defined __APPLE__
	#include <unistd.h>
	#include <sys/time.h>
	#include <termios.h>
	#include <fcntl.h>
  #include <regex.h>
#endif

#if defined WIN32 || defined WIN64
	#include <Windows.h>
  #include <regex>
#endif


// http://stackoverflow.com/questions/1413445/read-a-password-from-stdcin
void SetStdinEcho(bool enable = true) {
#if defined WIN32 || defined WIN64
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if( !enable )
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;

    SetConsoleMode(hStdin, mode);
#endif
#if defined __gnu_linux__ || defined __APPLE__
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if( !enable )
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

bool testForComment(std::string& line) {
  #if defined WIN32 || defined WIN64
    std::regex testRegex("^//.*$", std::regex_constants::extended);
    return std::regex_match(line, testRegex);
  #endif

  // stdlibc++ wasn't feature complete for C++11 at the time writing this code
  // FIXME: if c++ lib supports this, remove the following code

  #if defined __gnu_linux__ || defined __APPLE__
    int rv;
    regex_t * exp = new regex_t;
    rv = regcomp(exp, "^//.*", REG_EXTENDED);
    if (rv != 0) {
      std::cout << "regcomp failed with " << rv << std::endl;
    }
    bool match = regexec(exp, line.c_str(), 0, NULL, 0) == 0;
    regfree(exp);
    return match;
  #endif
}

void readQuestions(std::ifstream& questionFile, std::vector<std::string>& questions) {
  while(questionFile) {
    std::string line;
    std::getline(questionFile, line);
    if (line.length() > 0 && !testForComment(line))
      questions.push_back(line);
  }
  std::sort(questions.begin(), questions.end());
}

unsigned int getRnd(void) {
  unsigned int rndNumber = 0;
#if defined __gnu_linux__ || defined __APPLE__
  int urandom = open("/dev/urandom", O_RDONLY);
  assert(urandom);
  read(urandom, &rndNumber, sizeof(unsigned int));
  close(urandom);
#endif
#ifdef WIN32
  HCRYPTPROV hprovider = 0;
  CryptAcquireContext(&hprovider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
  CryptGenRandom(hprovider, sizeof(unsigned int), reinterpret_cast<byte*>(&rndNumber));
  CryptReleaseContext(hprovider, 0);
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

  typedef std::vector<std::string> StringVector;
  typedef std::shared_ptr<StringVector> StringVector_Ptr;

  std::string filename(argv[1]);
  StringVector_Ptr questions(new StringVector);

  std::ifstream questionFile(filename.c_str());
  if (questionFile.good())
	  readQuestions(questionFile, *questions);
  else {
      std::cerr << "Can't open given file " << filename << std::endl;
      return EXIT_FAILURE;
  }
  questionFile.close();

  // Do we have any questions? % todo->size is DIV_BY_ZERO otherwise
  if (questions->size() == 0) {
	  std::cerr << "No question provided!" << std::endl;
	  return EXIT_FAILURE;
  }

  StringVector_Ptr todo(new StringVector(*questions));
  StringVector_Ptr done(new StringVector);

  typedef std::chrono::high_resolution_clock clock;
  typedef std::chrono::milliseconds milliseconds;
  clock::time_point t0;
  clock::time_point t1;
  std::string question;
  int id = 0;
  while(true) {
    id = getRnd() % todo->size();
    question = (*todo)[id];
    todo->erase(std::find(todo->begin(), todo->end(), question));
    done->push_back(question);

    std::cout << question << std::endl;
	SetStdinEcho(false);
	t0 = clock::now();
    std::cin.get();
	t1 = clock::now();
    SetStdinEcho(true);
    milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
    std::cout <<  total_ms.count() / 1000.0 << " seconds for answer!" << std::endl;
    if (todo->empty()) {
        std::swap(todo, done);
        std::cout << "### Complete, starting again. ###" << std::endl;
    }
  }

  return EXIT_SUCCESS;
}
