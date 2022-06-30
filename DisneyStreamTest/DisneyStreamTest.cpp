// DisneyStreamTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <urlmon.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string.h>
#include "DisneyStreamTest.h"

#pragma comment(lib, "urlmon.lib")

int main()
{
    //1. Fetch file from URL
    std::string url = "https://lw.bamgrid.com/2.0/hls/vod/bam/ms02/hls/dplus/bao/master_unenc_hdr10_all.m3u8";
    std::string savepath = "C:\\Users\\Chris\\Downloads\\master_unenc_hdr10_all.m3u8";

    //download the file and if successful continue
    if (fetchFile(url, savepath) == S_OK)
    {
        //vectors to store each tag type
        std::vector<ext_media> mediaVec;
        std::vector<ext_stream> streamVec;
        std::vector<ext_iframe> iframeVec;

        //2. Parse File and sort by tag type
        parseFile(savepath, mediaVec, streamVec, iframeVec);


        //3. serve information to user, allow sorting options
        printTag(mediaVec, streamVec, iframeVec);
    }
}

/*
* Downloads a file from a url to a folder
*/
HRESULT fetchFile(std::string url, std::string savepath)
{
    std::wstring urlTemp = std::wstring(url.begin(), url.end());
    std::wstring savepathTemp = std::wstring(savepath.begin(), savepath.end());
    return URLDownloadToFile(NULL, urlTemp.c_str(), savepathTemp.c_str(), 0, NULL);;
}

/*
* open a file and loop through line by line, parsing the tag and determining if it is one of the tags we are parsing (tags with attributes)
*/
void parseFile(std::string filepath, std::vector<ext_media>& mediaVec, std::vector<ext_stream>& streamVec, std::vector<ext_iframe>& iframeVec)
{
    std::ifstream infile(filepath);
    std::string line;
    //loop through file line by line
    while (std::getline(infile, line))
    {
        //check if string is empty to avoid accessing nonexistent character
        if (line.empty())
        {
            continue;
        }
        //check if line has a tag
        if (line[0] == '#')
        {
            size_t found = line.find(':');
            if (found != std::string::npos)
            {
                std::string lineType = line.substr(0, found);
                line.erase(0, found + 1);
                if (lineType == "#EXT-X-MEDIA")
                {
                    ext_media mediaLine = parseExtMedia(line);
                    mediaVec.push_back(mediaLine);
                }
                else if (lineType == "#EXT-X-STREAM-INF")
                {
                    ext_stream streamLine = parseExtStream(line);
                    std::getline(infile, line); //the line after an EXT-STREAM line is the file path and needs to be included in the current struct
                    streamLine.filepath = line;
                    streamVec.push_back(streamLine);
                }
                else if (lineType == "#EXT-X-I-FRAME-STREAM-INF")
                {
                    ext_iframe iframeLine = parseExtIframe(line);
                    iframeVec.push_back(iframeLine);
                }
            }
        }
    }
}

/*
* Parse EXT-MEDIA tag into attributes
*/
ext_media parseExtMedia(std::string mediaLine)
{
    size_t pos = 0;
    ext_media output;
    while ((pos = mediaLine.find(',')) != std::string::npos)
    {
        std::string field = mediaLine.substr(0, pos);
        mediaLine.erase(0, pos + 1);
        addMediaField(field, output);
    }
    addMediaField(mediaLine, output);
    return output;
}

/*
* Build ext media struct
*/
void addMediaField(std::string field, ext_media& media)
{
    size_t equalPos = field.find('=');
    std::string fieldName = field.substr(0, equalPos);
    std::string fieldValue = field.substr(equalPos + 1);
    if (fieldName == fieldValue)
    {
        fieldName = m_lastField;
        fieldValue = "," + fieldValue;
    }
    fieldValue.erase(remove(fieldValue.begin(), fieldValue.end(), '"'), fieldValue.end());
    if (fieldName == "TYPE")
    {
        media.type += fieldValue;
        m_lastField = "TYPE";
    }
    else if (fieldName == "GROUP-ID")
    {
        media.groupID += fieldValue;
        m_lastField = "GROUP-ID";
    }
    else if (fieldName == "NAME")
    {
        media.name += fieldValue;
        m_lastField = "NAME";
    }
    else if (fieldName == "LANGUAGE")
    {
        media.language += fieldValue;
        m_lastField = "LANGUAGE";
    }
    else if (fieldName == "DEFAULT")
    {
        media.def += fieldValue;
        m_lastField = "DEFAULT";
    }
    else if (fieldName == "AUTOSELECT")
    {
        media.autoselect += fieldValue;
        m_lastField = "AUTOSELECT";
    }
    else if (fieldName == "CHANNELS")
    {
        media.channels += fieldValue;
        m_lastField = "CHANNELS";
    }
    else if (fieldName == "URI")
    {
        media.uri += fieldValue;
        m_lastField = "URI";
    }
}

/*
* Parse EXT-STREAM tag into attributes
*/
ext_stream parseExtStream(std::string streamLine)
{
    size_t pos = 0;
    ext_stream output;
    while ((pos = streamLine.find(',')) != std::string::npos)
    {
        std::string field = streamLine.substr(0, pos);
        streamLine.erase(0, pos + 1);
        addStreamField(field, output);
    }
    addStreamField(streamLine, output);
    return output;
}

/*
* Build ext stream struct
*/
void addStreamField(std::string field, ext_stream& stream)
{
    size_t equalPos = field.find('=');
    std::string fieldName = field.substr(0, equalPos);
    std::string fieldValue = field.substr(equalPos + 1);
    if (fieldName == fieldValue)
    {
        fieldName = m_lastField;
        fieldValue = "," + fieldValue;
    }
    fieldValue.erase(remove(fieldValue.begin(), fieldValue.end(), '"'), fieldValue.end());
    if (fieldName == "BANDWIDTH")
    {
        stream.bandwidth += fieldValue;
        m_lastField = "BANDWIDTH";
    }
    else if (fieldName == "AVERAGE-BANDWIDTH")
    {
        stream.averageBandwidth += fieldValue;
        m_lastField = "AVERAGE-BANDWIDTH";
    }
    else if (fieldName == "CODECS")
    {
        stream.codecs += fieldValue;
        m_lastField = "CODECS";
    }
    else if (fieldName == "RESOLUTION")
    {
        stream.resolution += fieldValue;
        m_lastField = "RESOLUTION";
    }
    else if (fieldName == "FRAME-RATE")
    {
        stream.frameRate += fieldValue;
        m_lastField = "FRAME-RATE";
    }
    else if (fieldName == "VIDEO-RANGE")
    {
        stream.videoRange += fieldValue;
        m_lastField = "VIDEO-RANGE";
    }
    else if (fieldName == "AUDIO")
    {
        stream.audio += fieldValue;
        m_lastField = "AUDIO";
    }
    else if (fieldName == "CLOSED-CAPTIONS")
    {
        stream.closedCaptions += fieldValue;
        m_lastField = "CLOSED-CAPTIONS";
    }
}

/*
* Parse EXT-IFRAME tag into attributes
*/
ext_iframe parseExtIframe(std::string iframeLine)
{
    size_t pos = 0;
    ext_iframe output;
    while ((pos = iframeLine.find(',')) != std::string::npos)
    {
        std::string field = iframeLine.substr(0, pos);
        iframeLine.erase(0, pos + 1);
        addIframeField(field, output);
    }
    addIframeField(iframeLine, output);
    return output;
}

/*
* Build ext iframe struct
*/
void addIframeField(std::string field, ext_iframe& iframe)
{
    size_t equalPos = field.find('=');
    std::string fieldName = field.substr(0, equalPos);
    std::string fieldValue = field.substr(equalPos + 1);
    if (fieldName == fieldValue)
    {
        fieldName = m_lastField;
        fieldValue = "," + fieldValue;
    }
    fieldValue.erase(remove(fieldValue.begin(), fieldValue.end(), '"'), fieldValue.end());
    if (fieldName == "BANDWIDTH")
    {
        iframe.bandwidth += fieldValue;
        m_lastField = "BANDWIDTH";
    }
    else if (fieldName == "CODECS")
    {
        iframe.codecs += fieldValue;
        m_lastField = "CODECS";
    }
    else if (fieldName == "RESOLUTION")
    {
        iframe.resolution += fieldValue;
        m_lastField = "RESOLUTION";
    }
    else if (fieldName == "VIDEO-RANGE")
    {
        iframe.videoRange += fieldValue;
        m_lastField = "VIDEO-RANGE";
    }
    else if (fieldName == "URI")
    {
        iframe.uri += fieldValue;
        m_lastField = "URI";
    }
}

/*
* Print a list of a tag
*/
void printTag(std::vector<ext_media>& mediaVec, std::vector<ext_stream>& streamVec, std::vector<ext_iframe>& iframeVec)
{
    std::cout << "Pick a tag (Input 1-3):" << "\n";
    std::cout << "1. #EXT-X-MEDIA" << "\n";
    std::cout << "2. #EXT-X-STREAM-INF" << "\n";
    std::cout << "3. #EXT-X-I-FRAME-STREAM-INF" << "\n";

    //GET INPUT 1-3
    int input = getIntInput(1, 3);

    switch (input)
    {
    case 1:
        //EXT-X-MEDIA
        sortAndPrintMedia(mediaVec);
        break;
    case 2:
        //EXT-X-STREAM-INF
        sortAndPrintStream(streamVec);
        break;
    case 3:
        //EXT-X-I-FRAME-STREAM-INF
        sortAndPrintIframe(iframeVec);
        break;
    default:
        break;
    }
}

/*
* Sort and Print a list of media tags
*/
void sortAndPrintMedia(std::vector<ext_media>& mediaVec)
{
    std::cout << "How would you like to sort?" << "\n";
    std::cout << "1. Group-ID" << "\n";
    std::cout << "2. Channels" << "\n";

    //GET INPUT 1-2
    int input = getIntInput(0, 2);
    switch (input)
    {
    case 1:
        //Group-ID
        std::sort(mediaVec.begin(), mediaVec.end(), compareByGroupIDMedia);
        break;
    case 2:
        //Channels
        std::sort(mediaVec.begin(), mediaVec.end(), compareByChannelsMedia);
        break;
    default:
        break;
    }

    for (ext_media media : mediaVec)
    {
        std::cout << media << "\n";
    }

}

/*
* Sort and Print a list of stream tags
*/
void sortAndPrintStream(std::vector<ext_stream>& streamVec)
{
    std::cout << "How would you like to sort?" << "\n";
    std::cout << "1. Audio" << "\n";
    std::cout << "2. Bandwidth" << "\n";
    std::cout << "3. Average Bandwidth" << "\n";
    std::cout << "4. Resolution (High to Low)" << "\n";

    //GET INPUT 1-4
    int input = getIntInput(0, 4);
    switch (input)
    {
    case 1:
        //Audio
        std::sort(streamVec.begin(), streamVec.end(), compareByAudioStream);
        break;
    case 2:
        //Bandwidth
        std::sort(streamVec.begin(), streamVec.end(), compareByBandwidthStream);
        break;
    case 3:
        //Average Bandwidth
        std::sort(streamVec.begin(), streamVec.end(), compareByAverageBandwidthStream);
        break;
    case 4:
        //Resolution (High to Low)
        std::sort(streamVec.begin(), streamVec.end(), compareByResolutionHighToLowStream);
        break;
    default:
        break;
    }

    for (ext_stream stream : streamVec)
    {
        std::cout << stream << "\n";
    }
}

/*
* Sort and Print a list of stream tags
*/
void sortAndPrintIframe(std::vector<ext_iframe>& iframeVec)
{
    std::cout << "How would you like to sort?" << "\n";
    std::cout << "1. Bandwidth" << "\n";
    std::cout << "2. Resolution (High to Low)" << "\n";

    //GET INPUT 1-2
    int input = getIntInput(0, 2);
    switch (input)
    {
    case 1:
        //Bandwidth
        std::sort(iframeVec.begin(), iframeVec.end(), compareByBandwidthIframe);
        break;
    case 2:
        //Resolution (High to Low)
        std::sort(iframeVec.begin(), iframeVec.end(), compareByResolutionHighToLowIframe);
        break;
    default:
        break;
    }

    for (ext_iframe iframe : iframeVec)
    {
        std::cout << iframe << "\n";
    }
}

/*
* Get an int input between 2 values
*/
int getIntInput(int min, int max)
{
    int a;
    std::cin >> a;
    while (std::cin.fail() || (a < min) || (a > max))
    {
        std::cout << "Invalid input, please try again." << "\n";
        std::cin >> a;
    }
    std::cout << "\n";
    return a;
}

//comparison functions for ext-stream type
bool compareByAudioStream(const ext_stream& a, const ext_stream& b)
{
    return a.audio < b.audio;
}
bool compareByBandwidthStream(const ext_stream& a, const ext_stream& b)
{
    return stoi(a.bandwidth) < stoi(b.bandwidth);
}
bool compareByAverageBandwidthStream(const ext_stream& a, const ext_stream& b)
{
    return stoi(a.averageBandwidth) < stoi(b.averageBandwidth);
}
bool compareByResolutionHighToLowStream(const ext_stream& a, const ext_stream& b)
{
    return getPixels(a.resolution) > getPixels(b.resolution);
}

//comparison functions for ext-media type
bool compareByGroupIDMedia(const ext_media& a, const ext_media& b)
{
    return a.groupID < b.groupID;
}
bool compareByChannelsMedia(const ext_media& a, const ext_media& b)
{
    return stoi(a.channels) < stoi(b.channels);
}

//comparison functions for ext-iframe type
bool compareByBandwidthIframe(const ext_iframe& a, const ext_iframe& b)
{
    return stoi(a.bandwidth) < stoi(b.bandwidth);
}
bool compareByResolutionHighToLowIframe(const ext_iframe& a, const ext_iframe& b)
{
    return getPixels(a.resolution) > getPixels(b.resolution);
}

//takes a string resolution a x b and multiplies the two values together to be used for size comparison
int getPixels(std::string resolution)
{
    size_t x = resolution.find('x');
    int a = stoi(resolution.substr(0, x));
    int b = stoi(resolution.substr(x + 1));
    return a * b;
}