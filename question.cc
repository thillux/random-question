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

void readQuestions(std::string questionFileName, std::vector<std::string>& questions) {
  std::ifstream questionFile(questionFileName);

  while(questionFile) {
    std::string line;
    std::getline(questionFile, line);
    if (line.length() > 1)
      questions.push_back(line);
  }

  std::sort(questions.begin(), questions.end());
}

unsigned int getRnd(void) {
  int urandom = open("/dev/urandom", O_RDONLY);
  assert(urandom);
  unsigned int rndNumber;
  read(urandom, &rndNumber, sizeof(unsigned int));
  close(urandom);
  return rndNumber;
}

int main(void) {
  const std::string FILENAME("questions");
  std::vector<std::string> * questions = new std::vector<std::string>;

  readQuestions(FILENAME, *questions);

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
