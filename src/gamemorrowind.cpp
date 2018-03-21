#include "gamemorrowind.h"

#include "morrowindbsainvalidation.h"
#include "morrowindscriptextender.h"
#include "morrowinddataarchives.h"
#include "morrowindsavegameinfo.h"
#include "morrowindgameplugins.h"

#include "executableinfo.h"
#include "pluginsetting.h"

#include <gamebryolocalsavegames.h>
#include <gamebryounmanagedmods.h>

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>

#include <QtDebug>

#include <Windows.h>
#include <winver.h>

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace MOBase;

GameMorrowind::GameMorrowind()
{
}

bool GameMorrowind::init(IOrganizer *moInfo)
{
  if (!GameGamebryo::init(moInfo)) {
    return false;
  }
  registerFeature<ScriptExtender>(new MorrowindScriptExtender(this));
  registerFeature<DataArchives>(new MorrowindDataArchives(gameDirectory().absolutePath()));
  registerFeature<BSAInvalidation>(new MorrowindBSAInvalidation(feature<DataArchives>(), this));
  registerFeature<SaveGameInfo>(new MorrowindSaveGameInfo(this));
  registerFeature<LocalSavegames>(new GamebryoLocalSavegames(gameDirectory().absolutePath(), "morrowind.ini"));
  registerFeature<GamePlugins>(new MorrowindGamePlugins(moInfo));
  registerFeature<UnmanagedMods>(new GamebryoUnmangedMods(this));
  return true;
}

QString GameMorrowind::gameName() const
{
  return "Morrowind";
}

QString GameMorrowind::getLauncherName() const
{
  return "Morrowind Launcher.exe";
}

QDir GameMorrowind::dataDirectory() const
{
  return gameDirectory().absoluteFilePath("Data Files");
}

QDir GameMorrowind::savesDirectory() const
{
  return QDir(gameDirectory().absoluteFilePath("Saves"));
}

QDir GameMorrowind::documentsDirectory() const
{
  return gameDirectory();
}

QList<ExecutableInfo> GameMorrowind::executables() const
{
  return QList<ExecutableInfo>()
  //    << ExecutableInfo("MWSE", findInGameFolder(feature<ScriptExtender>()->loaderName()))
      << ExecutableInfo("Morrowind", findInGameFolder(binaryName()))
      << ExecutableInfo("Morrowind Launcher", findInGameFolder(getLauncherName()))
	  << ExecutableInfo("MGE XE", findInGameFolder("MGEXEgui.exe"))
  ;
}

QString GameMorrowind::name() const
{
  return "Morrowind Support Plugin";
}

QString GameMorrowind::author() const
{
  return "Schilduin";
}

QString GameMorrowind::description() const
{
  return tr("Adds support for the game Morrowind");
}

MOBase::VersionInfo GameMorrowind::version() const
{
  return VersionInfo(0, 1, 2, VersionInfo::RELEASE_FINAL);
}

bool GameMorrowind::isActive() const
{
  return qApp->property("managed_game").value<IPluginGame*>() == this;
}

QList<PluginSetting> GameMorrowind::settings() const
{
  return QList<PluginSetting>();
}

void GameMorrowind::initializeProfile(const QDir &path, ProfileSettings settings) const
{
  if (settings.testFlag(IPluginGame::MODS)) {
    copyToProfile(localAppFolder() + "/Morrowind", path, "plugins.txt");
    copyToProfile(localAppFolder() + "/Morrowind", path, "loadorder.txt");
  }

  if (settings.testFlag(IPluginGame::CONFIGURATION)) {
    if (settings.testFlag(IPluginGame::PREFER_DEFAULTS)
        || !QFileInfo(myGamesPath() + "/morrowind.ini").exists()) {
      copyToProfile(gameDirectory().absolutePath(), path, "Morrowind.ini");
    } else {
      copyToProfile(myGamesPath(), path, "Morrowind.ini");
    }
  }
}

QString GameMorrowind::savegameExtension() const
{
  return "ess";
}

QString GameMorrowind::savegameSEExtension() const
{
  return "mwse";
}

QString GameMorrowind::steamAPPId() const
{
  return "22320";
}

QStringList GameMorrowind::primaryPlugins() const
{
  return { "Morrowind.esm" };
}

QString GameMorrowind::binaryName() const
{
  return "Morrowind.exe";
}

QString GameMorrowind::gameShortName() const
{
  return "Morrowind";
}

QString GameMorrowind::gameNexusName() const
{
  return "Morrowind";
}


QStringList GameMorrowind::iniFiles() const
{
  return { "morrowind.ini" };
}

QStringList GameMorrowind::DLCPlugins() const
{
  return { "Tribunal.esm", "Bloodmoon.esm" };
}

namespace {
//Note: This is ripped off from shared/util. And in an upcoming move, the fomod
//installer requires something similar. I suspect I should abstract this out
//into gamebryo (or lower level)

VS_FIXEDFILEINFO GetFileVersion(const std::wstring &fileName)
{
  DWORD handle = 0UL;
  DWORD size = ::GetFileVersionInfoSizeW(fileName.c_str(), &handle);
  if (size == 0) {
    throw std::runtime_error("failed to determine file version info size");
  }

  std::vector<char> buffer(size);
  handle = 0UL;
  if (!::GetFileVersionInfoW(fileName.c_str(), handle, size, buffer.data())) {
    throw std::runtime_error("failed to determine file version info");
  }

  void *versionInfoPtr = nullptr;
  UINT versionInfoLength = 0;
  if (!::VerQueryValue(buffer.data(), L"\\", &versionInfoPtr, &versionInfoLength)) {
    throw std::runtime_error("failed to determine file version");
  }

  return *static_cast<VS_FIXEDFILEINFO*>(versionInfoPtr);
}

}


int GameMorrowind::nexusModOrganizerID() const
{
  return 1334;
}

int GameMorrowind::nexusGameID() const
{
  return 100;
}
