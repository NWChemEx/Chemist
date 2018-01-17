pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                echo 'Building...'
	        sh '''
		set +x
	        source /etc/profile
  	        module load gcc/7.1.0-4bgguyp
     	        module load cmake
	        cmake -H. -Bbuild
	        cd build
	        make
	        '''
            }
        }
        stage('Test') {
            steps {
                echo 'Testing..'
	  	sh'''
		set +x
     	        source /etc/profile
	        module load cmake
	        cd build
	        ctest
	        '''
            }
        }
    }
}
