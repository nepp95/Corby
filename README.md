# Engine

Visual studio 2019/2022 is required. Anything else is untested, but may work.

**Instructions**
1. `git clone --recursive https://github.com/nepp95/Engine`
2. IF you have Visual studio 2022, continue to step 3.
	2a. Edit the `scripts/Win-GenProjects.bat` file.
	2b. Change `vs2022` to the right version. Possible versions are available at: https://premake.github.io/docs/Using-Premake
3. Run the `scripts/Setup.bat` file. This will download all prerequisites for the project.
4. If VulkanSDK was not installed prior to running the setup, please run the setup again after it is finished.
5. If all went well, you should now have your project/solution files ready to go!
