// voxel_mesher
//
// Copyright 2017 Chris Richardson
//
// This software is licenced under the Gnu LGPLv3 https://www.gnu.org/licenses/lgpl-3.0
//

#include "raw.h"

#include <boost/algorithm/string.hpp>
#include <cassert>
#include <fstream>

//-----------------------------------------------------------------------------
void read_raw(const std::string filename, CGAL::Image_3& image)
{
  std::ifstream datfile(filename);

  std::string object_filename;
  std::string format;
  std::vector<int> res(3);
  std::vector<double> dx = {1.0, 1.0, 1.0};

  while(datfile.is_open())
  {
    std::string line;
    while(std::getline(datfile, line))
    {
      if (line[0] == '#')
        std::cout << line << "\n";
      else
      {
        std::vector<std::string> vline;
        boost::split(vline, line, boost::is_any_of(":"));
        if(vline.size() > 0)
        {
          if (vline[0]=="ObjectFileName")
          {
            boost::trim(vline[1]);
            object_filename = vline[1];
          }
          else if (vline[0] == "Resolution")
          {
            boost::trim(vline[1]);
            std::vector<std::string> res_str;
            boost::split(res_str, vline[1], boost::is_any_of(" "));
            assert(res_str.size() == 3);
            for (int i = 0; i < 3; ++i)
              res[i] = std::stoul(res_str[i]);
            std::cout << "Resolution:" << res[0] << "x" << res[1] << "x" << res[2] << "\n";
          }
          else if (vline[0] == "Format")
          {
            boost::trim(vline[1]);
            format = vline[1];
          }
          else if (vline[0] == "SliceThickness")
          {
            boost::trim(vline[1]);
            std::vector<std::string> dx_str;
            boost::split(dx_str, vline[1], boost::is_any_of(" "));
            assert(dx_str.size() == 3);
            for (int i = 0; i < 3; ++i)
              dx [i] = std::stod(dx_str[i]);
            std::cout << "dx:" << dx[0] << "x" << dx[1] << "x" << dx[2] << "\n";
          }

        }
      }
    }
    datfile.close();
  }


  image = _createImage(res[0], res[1], res[2], 1,
                       dx[0], dx[1], dx[2], 1,
                       WK_FIXED, SGN_UNSIGNED);

  std::cout << "filename = '" << object_filename << "'\n";
  std::cout << "format = '" << format << "'\n";

  int width = -1;

  if (format == "USHORT")
    width = 2;
  else if (format == "UCHAR")
    width = 1;

  assert (width != -1);

  std::ifstream binfile;
  binfile.open(object_filename, std::ios::in | std::ios::binary);
  binfile.seekg(0, std::ios::end);
  int length = binfile.tellg();
  std::cout << "length = " << length << "\n";
  assert(length == res[0]*res[1]*res[2]*width);

  binfile.seekg(0, std::ios::beg);
  std::vector<char> buffer(length);
  binfile.read(buffer.data(), length);
  binfile.close();

  // Convert to 8-bit
  char *iptr = (char *)(image.data());
  char *bptr = buffer.data();
  for (std::size_t i = 0; i != res[0]*res[1]*res[2]; ++i)
    iptr[i] = bptr[(i + 1)*width - 1];

}
