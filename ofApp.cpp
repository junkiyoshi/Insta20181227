#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetColor(255);
	ofEnableDepthTest();
	ofDisableArbTex();

	this->cap.open("video.mp4");
	this->cap_size = cv::Size(640, 360);

	this->rect_size = 40;
	for (int x = 0; x < this->cap_size.width; x += this->rect_size) {

		for (int y = 0; y < this->cap_size.height; y += this->rect_size) {

			auto image = make_unique<ofImage>();
			image->allocate(this->rect_size, this->rect_size, OF_IMAGE_COLOR);
			cv::Mat frame = cv::Mat(cv::Size(image->getWidth(), image->getHeight()), CV_MAKETYPE(CV_8UC3, image->getPixels().getNumChannels()), image->getPixels().getData(), 0);
			cv::Rect rect = cv::Rect(x, y, this->rect_size, this->rect_size);

			this->rect_images.push_back(move(image));
			this->rect_frames.push_back(frame);
			this->cv_rects.push_back(rect);
		}
	}

	this->box.set(this->rect_size * 0.8);
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	cv::Mat src;
	this->cap >> src;
	if (src.empty()) {

		this->cap.set(cv::CAP_PROP_POS_FRAMES, 1);
		return;
	}

	cv::resize(src, this->frame, this->cap_size);
	cv::cvtColor(this->frame, this->frame, cv::COLOR_BGR2RGB);
	for (int i = 0; i < this->rect_images.size(); i++) {

		cv::Mat tmp_box_image(this->frame, this->cv_rects[i]);
		tmp_box_image.copyTo(this->rect_frames[i]);

		this->rect_images[i]->update();
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();

	for (int i = 0; i < this->rect_images.size(); i++) {

		ofPushMatrix();
		ofTranslate(this->cv_rects[i].x - this->cap_size.width * 0.5 + this->rect_size * 0.5, this->cap_size.height * 0.5 - this->cv_rects[i].y - this->rect_size, 0);
		ofRotateX(ofNoise(this->cv_rects[i].x * 0.005 + ofGetFrameNum() * 0.003) * 720);
		ofRotateY(ofNoise(this->cv_rects[i].y * 0.005 + ofGetFrameNum() * 0.003) * 720);

		this->rect_images[i]->getTexture().bind();
		this->box.draw();
		this->rect_images[i]->getTexture().unbind();

		ofNoFill();
		ofDrawBox(this->rect_size * 0.8 + 1);
		ofPopMatrix();
	}

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}