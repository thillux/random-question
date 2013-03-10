#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <regex.h>

void readQuestions(std::ifstream& questionFile, std::vector<std::string>& questions) {
  int rv;
  regex_t * exp = new regex_t;
  rv = regcomp(exp, "^//.*", REG_EXTENDED);
  if (rv != 0) {
    std::cout << "regcomp failed with " << rv << std::endl;
  }
  while(questionFile) {
    std::string line;
    std::getline(questionFile, line);
    if (line.length() > 1 && regexec(exp, line.c_str(), 0, NULL, 0) == REG_NOMATCH)
      questions.push_back(line);
  }
  std::sort(questions.begin(), questions.end());
  regfree(exp);
}

unsigned int getRnd(void) {
  int urandom = open("/dev/urandom", O_RDONLY);
  assert(urandom);
  unsigned int rndNumber;
  read(urandom, &rndNumber, sizeof(unsigned int));
  close(urandom);
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
  while(true) {
    int id = getRnd() % todo->size();
    question = (*todo)[id];
    todo->erase(std::find(todo->begin(), todo->end(), question));
    done->push_back(question);

    std::cout << question << std::endl;

    if (todo->empty())
      std::swap(todo, done);

    std::cin.get();
  }

  delete todo;
  delete done;

  return EXIT_SUCCESS;
}
