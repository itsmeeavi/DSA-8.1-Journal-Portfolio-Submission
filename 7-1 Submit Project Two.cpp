#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// CS 300 Project Two: ABCU Advising Program
// This program loads course data from a CSV file, stores it in a map,
// prints an alphanumeric course list, and prints details for a selected course.

struct Course {
    std::string courseNumber;
    std::string courseTitle;
    std::vector<std::string> prerequisites;
};

// Remove leading and trailing spaces from a string.
std::string trim(const std::string& value) {
    size_t first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }

    size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

// Convert a string to uppercase so user input such as "csci400" works.
std::string toUpperCase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char character) { return static_cast<char>(std::toupper(character)); });
    return value;
}

// Split one CSV line by commas. The project input does not contain quoted commas.
std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream lineStream(line);
    std::string field;

    while (std::getline(lineStream, field, ',')) {
        fields.push_back(trim(field));
    }

    return fields;
}

// Load courses from a CSV file into the map data structure.
bool loadCourses(const std::string& fileName, std::map<std::string, Course>& courses) {
    std::ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        std::cout << "Error: Could not open file: " << fileName << std::endl;
        return false;
    }

    courses.clear();
    std::string line;
    int lineNumber = 0;

    while (std::getline(inputFile, line)) {
        ++lineNumber;
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        std::vector<std::string> fields = splitCsvLine(line);

        // Every valid course row must have at least a course number and title.
        if (fields.size() < 2 || fields.at(0).empty() || fields.at(1).empty()) {
            std::cout << "Warning: Skipping invalid line " << lineNumber << "." << std::endl;
            continue;
        }

        Course course;
        course.courseNumber = toUpperCase(fields.at(0));
        course.courseTitle = fields.at(1);

        for (size_t i = 2; i < fields.size(); ++i) {
            if (!fields.at(i).empty()) {
                course.prerequisites.push_back(toUpperCase(fields.at(i)));
            }
        }

        courses[course.courseNumber] = course;
    }

    inputFile.close();

    if (courses.empty()) {
        std::cout << "No course data was loaded." << std::endl;
        return false;
    }

    // Report any prerequisite course numbers that do not exist in the file.
    for (const auto& coursePair : courses) {
        const Course& course = coursePair.second;
        for (const std::string& prerequisite : course.prerequisites) {
            if (courses.find(prerequisite) == courses.end()) {
                std::cout << "Warning: " << course.courseNumber
                          << " lists missing prerequisite " << prerequisite << "." << std::endl;
            }
        }
    }

    std::cout << courses.size() << " courses loaded successfully." << std::endl;
    return true;
}

// Print the full course list in alphanumeric order. std::map keeps keys sorted.
void printCourseList(const std::map<std::string, Course>& courses) {
    if (courses.empty()) {
        std::cout << "Please load course data first." << std::endl;
        return;
    }

    std::cout << "Here is a sample schedule:" << std::endl << std::endl;

    for (const auto& coursePair : courses) {
        const Course& course = coursePair.second;
        std::cout << course.courseNumber << ", " << course.courseTitle << std::endl;
    }
}

// Print one course and its prerequisite information.
void printCourseInformation(const std::map<std::string, Course>& courses) {
    if (courses.empty()) {
        std::cout << "Please load course data first." << std::endl;
        return;
    }

    std::string courseNumber;
    std::cout << "What course do you want to know about? ";
    std::getline(std::cin, courseNumber);
    courseNumber = toUpperCase(trim(courseNumber));

    auto courseIterator = courses.find(courseNumber);

    if (courseIterator == courses.end()) {
        std::cout << "Course not found." << std::endl;
        return;
    }

    const Course& course = courseIterator->second;
    std::cout << course.courseNumber << ", " << course.courseTitle << std::endl;

    if (course.prerequisites.empty()) {
        std::cout << "Prerequisites: None" << std::endl;
        return;
    }

    std::cout << "Prerequisites: ";

    for (size_t i = 0; i < course.prerequisites.size(); ++i) {
        const std::string& prerequisiteNumber = course.prerequisites.at(i);
        std::cout << prerequisiteNumber;

        // Include the prerequisite title when the prerequisite exists in the data file.
        auto prerequisiteIterator = courses.find(prerequisiteNumber);
        if (prerequisiteIterator != courses.end()) {
            std::cout << " - " << prerequisiteIterator->second.courseTitle;
        }

        if (i < course.prerequisites.size() - 1) {
            std::cout << ", ";
        }
    }

    std::cout << std::endl;
}

void printMenu() {
    std::cout << std::endl;
    std::cout << "1. Load Data Structure." << std::endl;
    std::cout << "2. Print Course List." << std::endl;
    std::cout << "3. Print Course." << std::endl;
    std::cout << "9. Exit" << std::endl;
    std::cout << std::endl;
    std::cout << "What would you like to do? ";
}

int main() {
    std::map<std::string, Course> courses;
    std::string userInput;
    int menuChoice = 0;

    std::cout << "Welcome to the course planner." << std::endl;

    while (menuChoice != 9) {
        printMenu();
        std::getline(std::cin, userInput);

        try {
            menuChoice = std::stoi(trim(userInput));
        }
        catch (...) {
            menuChoice = -1;
        }

        switch (menuChoice) {
        case 1: {
            std::string fileName;
            std::cout << "Enter the course data file name: ";
            std::getline(std::cin, fileName);
            fileName = trim(fileName);

            if (fileName.empty()) {
                fileName = "CS 300 ABCU_Advising_Program_Input.csv";
                std::cout << "No file entered. Using default: " << fileName << std::endl;
            }

            loadCourses(fileName, courses);
            break;
        }
        case 2:
            printCourseList(courses);
            break;
        case 3:
            printCourseInformation(courses);
            break;
        case 9:
            std::cout << "Thank you for using the course planner!" << std::endl;
            break;
        default:
            std::cout << menuChoice << " is not a valid option." << std::endl;
            break;
        }
    }

    return 0;
}
