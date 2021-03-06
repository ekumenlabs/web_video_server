#include "web_video_server/jpeg_streamers.h"
#include "async_web_server_cpp/http_reply.hpp"

namespace web_video_server
{

MjpegStreamer::MjpegStreamer(const async_web_server_cpp::HttpRequest &request,
                             async_web_server_cpp::HttpConnectionPtr connection, image_transport::ImageTransport it) :
    ImageStreamer(request, connection, it)
{
  quality_ = request.get_query_param_value_or_default<int>("quality", 95);

  async_web_server_cpp::HttpReply::builder(async_web_server_cpp::HttpReply::ok).header("Connection", "close").header(
      "Server", "web_video_server").header("Cache-Control",
                                           "no-cache, no-store, must-revalidate, pre-check=0, post-check=0, max-age=0").header(
      "Pragma", "no-cache").header("Content-type", "multipart/x-mixed-replace;boundary=--boundarydonotcross ").header(
      "Access-Control-Allow-Origin", "*").header("Transfer-Encoding","chunked").write(connection);
  connection->write("15\r\n"); // size of hardcoded "--boundarydonotcross"	
  connection->write("--boundarydonotcross \r\n");
}

void MjpegStreamer::sendImage(const cv::Mat &img, const ros::Time &time)
{
  std::vector<int> encode_params;
  encode_params.push_back(CV_IMWRITE_JPEG_QUALITY);
  encode_params.push_back(quality_);

  std::vector<uchar> encoded_buffer;
  cv::imencode(".jpeg", img, encoded_buffer, encode_params);

  char stamp[20];
  sprintf(stamp, "%.06lf", time.toSec());
  boost::shared_ptr<std::vector<async_web_server_cpp::HttpHeader> > headers(
      new std::vector<async_web_server_cpp::HttpHeader>());
  headers->push_back(async_web_server_cpp::HttpHeader("Content-type", "image/jpeg"));
  headers->push_back(async_web_server_cpp::HttpHeader("X-Timestamp", stamp));
  headers->push_back(
     async_web_server_cpp::HttpHeader("Content-Length", boost::lexical_cast<std::string>(encoded_buffer.size())));
  
  std::vector<boost::asio::const_buffer> encoded_header = async_web_server_cpp::HttpReply::to_buffers(*headers); 
  std::vector<boost::asio::const_buffer>::iterator it;
  int length = 0;
  for(it=encoded_header.begin(); it !=encoded_header.end();it++){
    length+=buffer_size(*it);
  }
  length += 23; // 23 is the size of hardcodded "\r\n --boundarydonotcross"
  length += encoded_buffer.size();
  std::stringstream ss;
  ss << std::hex <<length;
  ss << "\r\n";
  connection_->write(ss.str());
  connection_->write(encoded_header, headers);
  connection_->write_and_clear(encoded_buffer);
  connection_->write("\r\n--boundarydonotcross \r\n");
}

boost::shared_ptr<ImageStreamer> MjpegStreamerType::create_streamer(const async_web_server_cpp::HttpRequest &request,
                                                                    async_web_server_cpp::HttpConnectionPtr connection,
                                                                    image_transport::ImageTransport it)
{
  return boost::shared_ptr<ImageStreamer>(new MjpegStreamer(request, connection, it));
}

std::string MjpegStreamerType::create_viewer(const async_web_server_cpp::HttpRequest &request)
{
  std::stringstream ss;
  ss << "<img src=\"/stream?";
  ss << request.query;
  ss << "\"></img>";
  return ss.str();
}

JpegSnapshotStreamer::JpegSnapshotStreamer(const async_web_server_cpp::HttpRequest &request,
                                           async_web_server_cpp::HttpConnectionPtr connection,
                                           image_transport::ImageTransport it) :
    ImageStreamer(request, connection, it)
{
  quality_ = request.get_query_param_value_or_default<int>("quality", 95);
}

void JpegSnapshotStreamer::sendImage(const cv::Mat &img, const ros::Time &time)
{
  std::vector<int> encode_params;
  encode_params.push_back(CV_IMWRITE_JPEG_QUALITY);
  encode_params.push_back(quality_);

  std::vector<uchar> encoded_buffer;
  cv::imencode(".jpeg", img, encoded_buffer, encode_params);

  char stamp[20];
  sprintf(stamp, "%.06lf", time.toSec());
  async_web_server_cpp::HttpReply::builder(async_web_server_cpp::HttpReply::ok).header("Connection", "close").header(
      "Server", "web_video_server").header("Cache-Control",
                                           "no-cache, no-store, must-revalidate, pre-check=0, post-check=0, max-age=0").header(
      "X-Timestamp", stamp).header("Pragma", "no-cache").header("Content-type", "image/jpeg").header(
      "Access-Control-Allow-Origin", "*").header("Content-Length",
                                                 boost::lexical_cast<std::string>(encoded_buffer.size())).write(
      connection_);
  connection_->write_and_clear(encoded_buffer);
  inactive_ = true;
}

}
