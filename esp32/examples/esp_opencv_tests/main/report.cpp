//
// Created by joachim on 01.04.20.
//

#include "report.hpp"
#include <utility>
#include <iomanip>

using namespace std;

// TestCase class
TestCase::TestCase(string _name, int _iterations, function<int(const cv::Mat &)> _func) :
        name(move(_name)), iterations(_iterations),  func(move(_func)) {}

TestCase::TestCase(string _name, function<int(const cv::Mat &)> _func) :
        TestCase(move(_name), DEFAULT_ITERATIONS, move(_func)) {}

void TestCase::executeOn(const imagesList &testImages) {
    for(auto img : testImages) {
        float timeTaken;
        try {
            timeTaken = (float) func(get<1>(img)) / 1000.0f;
        } catch(cv::Exception& e) {
            cout << "Exception while trying to execute function " << name << " on image " << get<0>(img) << " :\n" << e.msg;
            timeTaken = -1;
        }
        resultTime.push_back(timeTaken);
        cout << "Testing " << name << " on " << get<0>(img) << " took "
                  << resultTime.back() << " [ms] (in " << iterations << " iterations)" << endl;
    }
}

// TestGroup class
TestGroup::TestGroup(string _name) : name(move(_name)) {}

void TestGroup::addTestCase(const TestCase &test) {
    tests.push_back(test);
}

// Report class
Report::Report(string _name, const imagesList& _images) : name(move(_name)), images(_images), summary(_images) {}

TestGroup &Report::addGroup(const string &_name) {
    groups.push_back(TestGroup(_name));
    return groups.back();
}

TestGroup &Report::getGroup(const string &_name) {
    auto it = find_if(groups.begin(), groups.end(), [&_name] (const TestGroup& g) {return (g.name == _name);});
    if(it == groups.end()) {
        cout << "group not found!" << endl;
        return *it; // todo: exception?
    }
    else {
        return *it;
    }
}

void Report::startBenchmark() {
    cout << "starting benchmark with " << images.size() << " images, " << groups.size() << " groups" << endl;
    for(auto& group : groups) {
        cout << "Group contains " << group.tests.size() << " testcases" << endl;
        for(auto& test : group.tests) {
            test.executeOn(images);
        }
    }

}

string Report::getSummary() {
    string sum;
    sum += summary.summaryHeader(name);

    for(const auto& group : groups) {
        sum += summary.summarySection(group);
        for(const auto& test : group.tests) {
            sum += summary.summaryTest(test);
        }
    }
    return sum;
}


// Summary class
Summary::Summary(imagesList _images) : images(move(_images)) {
    // Fill the columns width vector
    columnsWidth.push_back(COL1_WIDTH);
    for(int i = 0; i < images.size(); i++) {
        columnsWidth.push_back(COL_VALUES_WIDTH);
    }
    // Get the total width  FIXME: columns width and alignment is a bit messy
    totalWidth = 0;
    for (auto& w : columnsWidth)
        totalWidth += (w-1);
    totalWidth += 1;
}

string Summary::lineSeparator(const char sep) {
    string line;

    for(int size : columnsWidth) {
        line += "+";
        line += string(size-2, sep);
    }

    line += "+\n";

    return line;
}

string Summary::alignTextLeft(const string &text, int colWidth) {
    string s;
    s += " " + text;
    s += string(colWidth - text.size() - 3, ' ');
    return s;
}

string Summary::alignTextRight(const string &text, int colWidth) {
    string s;
    s += string(colWidth - text.size() - 3, ' ');
    s += text + " ";
    return s;
}

string Summary::alignTextCenter(const string &text, int colWidth) {
    string s;
    int spaces = colWidth - text.size() - 2;
    s += string(spaces/2, ' ') + text + string(spaces/2, ' ');
    s += (spaces % 2 != 0) ? " " : "";
    return s;
}

string Summary::lineText(const vector<string> &colTexts) {
    string line;
    int numOfTextCol = colTexts.size();
    int remainingWidth = totalWidth;

    for(int c = 0; c < numOfTextCol; c++) {
        // if last text but not last column, fit it in the rest of the row
        line += "|";
        if((c == numOfTextCol-1) && (c < columnsWidth.size()-1)) {
            line += alignTextLeft(colTexts[c], remainingWidth);
        }
        else {
            // align text left on first col, right on the rest
            line += (c==0 ? alignTextLeft(colTexts[c], columnsWidth.at(c)) : alignTextRight(colTexts[c], columnsWidth.at(c)));
            remainingWidth -= (columnsWidth.at(c)-1);
        }
    }
    line += "|\n";
    return line;
}

string Summary::summaryHeader(const string &reportName) {
    string summaryHeaderLines;

    // extract images names
    vector<string> imagesNames;
    for(auto img : images) {
        imagesNames.push_back(get<0>(img));
    }
    imagesNames.insert(imagesNames.begin(), "");

    summaryHeaderLines += lineSeparator('-');
    summaryHeaderLines += lineText({"Function name and arguments", reportName});
    summaryHeaderLines += lineSeparator('-');
    summaryHeaderLines += lineText(imagesNames);
    summaryHeaderLines += lineSeparator('-');
    summaryHeaderLines += lineText({"Measures are in [ms]"});
    summaryHeaderLines += lineSeparator('=');
    return summaryHeaderLines;
}

string Summary::summarySection(const TestGroup &group) {
    string summarySectionLines;

    summarySectionLines += lineText({""});
    summarySectionLines += lineSeparator('-');
    summarySectionLines += lineText({"**" + group.name + "**"});
    summarySectionLines += lineSeparator('-');

    return summarySectionLines;
}

string Summary::summaryTest(const TestCase &test) {
    string summaryTestLines;
    vector<string> text;

    // fill text vector with name and values
    text.push_back(test.name);
    for(auto res : test.resultTime) {
        string value;
        if(res == -1) {
            value = " - ";
        }
        else {
            stringstream ss;
            ss << fixed << setprecision(((res < 10) ? 1 : 0)) << res;
            value = ss.str();
        }
        text.push_back(value);
    }
    summaryTestLines += lineText(text);
    summaryTestLines += lineSeparator('-');

    return summaryTestLines;
}

