#include "StringUtils.h"

std::vector<String> StringUtils::splitString(String input, char delimiter)
{
  std::vector<String> splitResults;
  int lastPos = 0;
  int nextPos = input.indexOf(delimiter);

  while (nextPos > 0)
  {
    splitResults.push_back(input.substring(lastPos, nextPos));
    lastPos = nextPos + 1;
    nextPos = input.indexOf(delimiter, nextPos + 1);
  }

  splitResults.push_back(input.substring(lastPos));

  return splitResults;
}

String StringUtils::joinStrings(std::vector<String> strings, char delimiter)
{
  String joinedStrings;
  for (uint i = 0; i < strings.size(); i++)
  {
    joinedStrings.concat(strings.at(i));
    if (i < strings.size() - 1)
    {
      joinedStrings.concat(delimiter);
    }
  }

  return joinedStrings;
}