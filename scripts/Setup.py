import os
import subprocess
import platform

# Make sure everything we need for the setup is installed
from SetupPython import PythonConfiguration as PythonRequirements
PythonRequirements.Validate()

from SetupVulkan import VulkanConfiguration as VulkanRequirements
os.chdir('./../') # Change from devtools/scripts directory to root
VulkanRequirements.Validate()

if platform.system() == "Windows":
    print("\nRunning premake...")
    subprocess.call([os.path.abspath("./scripts/Win-GenProjects.bat"), "nopause"])

print("\nSetup completed!")
