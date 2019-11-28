/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <iostream>
#include <string.h>

#include "sdf/sdf_config.h"
#include "sdf/Filesystem.hh"
#include "sdf/Frame.hh"
#include "sdf/FrameSemantics.hh"
#include "sdf/Joint.hh"
#include "sdf/Link.hh"
#include "sdf/Model.hh"
#include "sdf/Root.hh"
#include "sdf/World.hh"
#include "sdf/ign.hh"
#include "sdf/parser.hh"
#include "sdf/system_util.hh"

inline namespace SDF_VERSION_NAMESPACE {

//////////////////////////////////////////////////
// cppcheck-suppress unusedFunction
extern "C" SDFORMAT_VISIBLE int cmdCheck(const char *_path)
{
  int result = 0;

  sdf::Root root;
  sdf::Errors errors = root.Load(_path);
  if (!errors.empty())
  {
    for (auto &error : errors)
    {
      std::cerr << "Error: " << error.Message() << std::endl;
    }
    return -1;
  }

  if (!sdf::checkCanonicalLinkNames(&root))
  {
    std::cerr << "Error: invalid canonical link name.\n";
    result = -1;
  }

  if (!sdf::checkJointParentChildLinkNames(&root))
  {
    std::cerr << "Error: invalid parent or child link name.\n";
    result = -1;
  }

  if (!sdf::checkFrameAttachedToNames(&root))
  {
    std::cerr << "Error: invalid frame attached_to name.\n";
    result = -1;
  }

  if (!sdf::checkPoseRelativeToNames(&root))
  {
    std::cerr << "Error: invalid pose relative_to name.\n";
    result = -1;
  }

  if (!sdf::recursiveSiblingUniqueNames(root.Element()))
  {
    std::cerr << "Error: non-unique names detected.\n";
    result = -1;
  }

  if (!sdf::checkKinematicGraph(&root))
  {
    std::cerr << "Error: invalid kinematic graph.\n";
    result = -1;
  }

  if (!sdf::checkFrameAttachedToGraph(&root))
  {
    std::cerr << "Error: invalid frame attached_to graph.\n";
    result = -1;
  }

  if (!sdf::checkPoseRelativeToGraph(&root))
  {
    std::cerr << "Error: invalid pose relative_to graph.\n";
    result = -1;
  }

  if (!sdf::filesystem::exists(_path))
  {
    std::cerr << "Error: File [" << _path << "] does not exist.\n";
    return -1;
  }

  sdf::SDFPtr sdf(new sdf::SDF());

  if (!sdf::init(sdf))
  {
    std::cerr << "Error: SDF schema initialization failed.\n";
    return -1;
  }

  if (!sdf::readFile(_path, sdf))
  {
    std::cerr << "Error: SDF parsing the xml failed.\n";
    return -1;
  }

  if (result == 0)
  {
    std::cout << "Valid.\n";
  }
  return result;
}

//////////////////////////////////////////////////
// cppcheck-suppress unusedFunction
extern "C" SDFORMAT_VISIBLE char *ignitionVersion()
{
#ifdef _MSC_VER
  return _strdup(SDF_VERSION_FULL);
#else
  return strdup(SDF_VERSION_FULL);
#endif
}

//////////////////////////////////////////////////
/// \brief Print the full description of the SDF spec.
/// \return 0 on success, -1 if SDF could not be initialized.
// cppcheck-suppress unusedFunction
extern "C" SDFORMAT_VISIBLE int cmdDescribe()
{
  sdf::SDFPtr sdf(new sdf::SDF());

  if (!sdf::init(sdf))
  {
    std::cerr << "Error: SDF schema initialization failed.\n";
    return -1;
  }

  sdf->PrintDescription();

  return 0;
}

//////////////////////////////////////////////////
// cppcheck-suppress unusedFunction
extern "C" SDFORMAT_VISIBLE int cmdPrint(const char *_path)
{
  if (!sdf::filesystem::exists(_path))
  {
    std::cerr << "Error: File [" << _path << "] does not exist.\n";
    return -1;
  }

  sdf::SDFPtr sdf(new sdf::SDF());

  if (!sdf::init(sdf))
  {
    std::cerr << "Error: SDF schema initialization failed.\n";
    return -1;
  }

  if (!sdf::readFile(_path, sdf))
  {
    std::cerr << "Error: SDF parsing the xml failed.\n";
    return -1;
  }

  sdf->PrintValues();

  return 0;
}
}
