def buildModuleMatrix = [
    		   "GCC 7.3.0":("gcc/7.1.0-4bgguyp cmake/3.12.2-4qajrwt gcc_new/7.3.0")
		  ]
node{
    def nwxJenkins
    stage('Import Jenkins Commands'){
        sh """
           rm -rf ~/.cpp_cache
           da_url=https://raw.githubusercontent.com/NWChemEx-Project/
           da_url+=DeveloperTools/master/ci/Jenkins/nwxJenkins.groovy
           wget \${da_url}
           """
    	nwxJenkins=load("nwxJenkins.groovy")
    }
    nwxJenkins.commonSteps(buildModuleMatrix, "SDE")
}
