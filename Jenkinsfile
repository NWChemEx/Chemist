def buildModuleMatrix = [
    		   "GCC 7.3.0":("python/gcc-7.1.0 gcc/7.3.0-xyzezhj intel/18.0.3-d6gtsxs cmake/3.12.3-n3idxkw")
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
