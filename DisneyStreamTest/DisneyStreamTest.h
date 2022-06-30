#pragma once


//Structures used to store different tags in
//Each structure includes an overload for operator << so that the entire structure can be easily printed
struct ext_stream {
	std::string bandwidth;
	std::string averageBandwidth;
	std::string codecs;
	std::string resolution;
	std::string frameRate;
	std::string videoRange;
	std::string audio;
	std::string closedCaptions;
	std::string filepath;

};

std::ostream& operator<<(std::ostream& os, const ext_stream stream) {
	return os << "Bandwidth: " << "\t\t" << stream.bandwidth << "\n"
		<< "Average-Bandwidth: " << "\t" << stream.averageBandwidth << "\n"
		<< "Codecs: " << "\t\t" << stream.codecs << "\n"
		<< "Resolution: " << "\t\t" << stream.resolution << "\n"
		<< "Frame-Rate: " << "\t\t" << stream.frameRate << "\n"
		<< "Video-Range: " << "\t\t" << stream.videoRange << "\n"
		<< "Audio: " << "\t\t\t" << stream.audio << "\n"
		<< "Closed-Captions: " << "\t" << stream.closedCaptions << "\n"
		<< "Filepath: " << "\t\t" << stream.filepath << std::endl;
}

struct ext_media {
	std::string type;
	std::string groupID;
	std::string name;
	std::string language;
	std::string def;
	std::string autoselect;
	std::string channels;
	std::string uri;
};

std::ostream& operator<<(std::ostream& os, const ext_media media) {
	return os << "Type: " << "\t\t\t" << media.type << "\n"
		<< "Group-ID: " << "\t\t" << media.groupID << "\n"
		<< "Name: " << "\t\t\t" << media.name << "\n"
		<< "Language: " << "\t\t" << media.language << "\n"
		<< "Default: " << "\t\t" << media.def << "\n"
		<< "Autoselect: " << "\t\t" << media.autoselect << "\n"
		<< "Channels: " << "\t\t" << media.channels << "\n"
		<< "URI: " << "\t\t\t" << media.uri << std::endl;
}

struct ext_iframe {
	std::string bandwidth;
	std::string codecs;
	std::string resolution;
	std::string videoRange;
	std::string uri;
};

std::ostream& operator<<(std::ostream& os, const ext_iframe iframe) {
	return os << "Bandwidth: " << "\t\t" << iframe.bandwidth << "\n"
		<< "Codecs: " << "\t\t" << iframe.codecs << "\n"
		<< "Resolution: " << "\t\t" << iframe.resolution << "\n"
		<< "Video-Range: " << "\t\t" << iframe.videoRange << "\n"
		<< "URI: " << "\t\t\t" << iframe.uri << std::endl;
}

HRESULT fetchFile(std::string, std::string);
void parseFile(std::string, std::vector<ext_media>&, std::vector<ext_stream>&, std::vector<ext_iframe>&);

ext_media parseExtMedia(std::string);
void addMediaField(std::string, ext_media&);

ext_stream parseExtStream(std::string);
void addStreamField(std::string, ext_stream&);

ext_iframe parseExtIframe(std::string);
void addIframeField(std::string, ext_iframe&);

std::string m_lastField = "";

void printTag(std::vector<ext_media>&, std::vector<ext_stream>&, std::vector<ext_iframe>&);
void sortAndPrintMedia(std::vector<ext_media>&);
void sortAndPrintStream(std::vector<ext_stream>&);
void sortAndPrintIframe(std::vector<ext_iframe>&);
int getIntInput(int, int);

bool compareByAudioStream(const ext_stream&, const ext_stream&);
bool compareByBandwidthStream(const ext_stream&, const ext_stream&);
bool compareByAverageBandwidthStream(const ext_stream&, const ext_stream&);
bool compareByResolutionHighToLowStream(const ext_stream&, const ext_stream&);

bool compareByGroupIDMedia(const ext_media&, const ext_media&);
bool compareByChannelsMedia(const ext_media&, const ext_media&);

bool compareByBandwidthIframe(const ext_iframe&, const ext_iframe&);
bool compareByResolutionHighToLowIframe(const ext_iframe&, const ext_iframe&);

int getPixels(std::string);

