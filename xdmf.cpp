// voxel_mesher
//
// Copyright 2017 Chris Richardson
//
// This software is licenced under the Gnu LGPLv3 https://www.gnu.org/licenses/lgpl-3.0
//

#include "pugixml.hpp"
#include "xdmf.h"

#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>

void medit_to_xdmf(std::stringstream& medit_str, std::string filename)
{
  std::cout << medit_str.str() << "\n";

  std::string line;

  int mode = 0;

  std::string geom_str;
  std::string topo_str;
  std::size_t nvert, ncell;

  while(std::getline(medit_str, line))
  {
    std::vector<std::string> vline;
    boost::split(vline, line, boost::is_any_of(" \t"));

    if (vline.size() == 0)
      continue;
    else if (vline[0] == "Vertices")
    {
      mode = 1;
      std::getline(medit_str, line);
      nvert = std::stol(line);
      continue;
    }
    else if (vline[0] == "Triangles")
      mode = 2;
    else if (vline[0] == "Tetrahedra")
    {
      mode = 3;
      std::getline(medit_str, line);
      ncell = std::stol(line);
      continue;
    }
    else if (vline[0] == "End")
      mode = 4;

    if(mode == 1)
    {
      assert(vline.size() == 4);
      for (unsigned int j = 0; j < 3; ++j)
        geom_str += vline[j] + " ";
      geom_str += "\n";

    }
    else if(mode == 3)
    {
      assert(vline.size() == 5);
      for (unsigned int j = 0; j < 4; ++j)
      {
        std::size_t val = std::stol(vline[j]) - 1;
        topo_str += std::to_string(val)  + " ";
      }
      topo_str += "\n";

    }

  }

  pugi::xml_document xml_doc;

  xml_doc.append_child(pugi::node_doctype).set_value("Xdmf SYSTEM \"Xdmf.dtd\" []");
  pugi::xml_node xdmf_node = xml_doc.append_child("Xdmf");
  xdmf_node.append_attribute("Version") = "3.0";
  xdmf_node.append_attribute("xmlns:xi") = "http://www.w3.org/2001/XInclude";

  pugi::xml_node domain_node = xdmf_node.append_child("Domain");
  pugi::xml_node grid_node = domain_node.append_child("Grid");
  grid_node.append_attribute("Name") = "mesh";
  grid_node.append_attribute("GridType") = "Uniform";

  pugi::xml_node topology_node = grid_node.append_child("Topology");
  topology_node.append_attribute("NumberOfElements")
    = std::to_string(ncell).c_str();
  topology_node.append_attribute("TopologyType") = "Tetrahedron";
  topology_node.append_attribute("NodesPerElement") = "4";

  pugi::xml_node data_item_node = topology_node.append_child("DataItem");
  std::string dim = std::to_string(ncell) + " 4";

  data_item_node.append_attribute("Dimensions") = dim.c_str();
  data_item_node.append_attribute("NumberType") = "UInt";
  data_item_node.append_attribute("Format") = "XML";
  data_item_node.append_child(pugi::node_pcdata)
    .set_value(topo_str.c_str());

  pugi::xml_node geometry_node = grid_node.append_child("Geometry");
  geometry_node.append_attribute("GeometryType") = "XYZ";
  pugi::xml_node gdata_item_node = geometry_node.append_child("DataItem");
  dim = std::to_string(nvert) + " 3";
  gdata_item_node.append_attribute("Dimensions") = dim.c_str();
  gdata_item_node.append_attribute("NumberType") = "Float";
  gdata_item_node.append_attribute("Format") = "XML";
  gdata_item_node.append_child(pugi::node_pcdata)
    .set_value(geom_str.c_str());

  xml_doc.save_file(filename.c_str());
}
