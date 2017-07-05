;post-build command line:copy "$(TargetPath)" "$(SolutionDir)installer\res\$(TargetFileName)" /Y

Name "Adfilter"
OutFile "Adfilter.exe"


RequestExecutionLevel user


Section "section_name" section_index_output
    # your code here
    MessageBox MB_OK "新的親"
    
SectionEnd