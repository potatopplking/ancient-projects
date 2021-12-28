#!/usr/bin/env python

# void toCartesian(double longitude, double latitude, double altitude, Eigen::Vector3d &cartesian)
# {
# 	// constants - WGS84 ellipsoid parameters
# 	const double a = 6378137.0;
# 	const double e = 8.1819190842622e-2;

# 	double N = a/sqrt(1-e*e*pow(sin(latitude),2));
# 	double x = (N + altitude) * cos(latitude) * cos(longitude);
# 	double y = (N + altitude) * cos(latitude) * sin(longitude);
# 	double z = ((1 - e*e) * N + altitude) * sin(latitude);
# 	cartesian << x, y, z;
# }

import math

class Position():
	def __abs__(self):
		return math.sqrt(self.x**2 + self.y**2 + self.z**2)

	def __sub__(self,b):
		result = Position(0,0)
		result.x = self.x - b.x
		result.y = self.y - b.y
		result.z = self.z - b.z
		return result

	def __add__(self,b):
		result = Position(0,0)
		result.x = self.x + b.x
		result.y = self.y + b.y
		result.z = self.z + b.z
		return result

	def dot(a,b):
		x = a.x * b.x
		y = a.y * b.y
		z = a.z * b.z
		return x + y + z

	def toCartesian(self, latitude_deg, longitude_deg, altitude):
		# convert to radians
		latitude = latitude_deg/180.0*math.pi
		longitude = longitude_deg/180.0*math.pi
		# WGS84 ellipsoid parameters
		a = 6378137.0
		e = 8.1819190842622e-2
		N = a / math.sqrt(1-e*e*math.pow(math.sin(latitude),2))
		x = (N + altitude) * math.cos(latitude) * math.cos(longitude)
		y = (N + altitude) * math.cos(latitude) * math.sin(longitude)
		z = ((1 - e*e) * N + altitude) * math.sin(latitude)
		return x, y, z

	def updateECEF(self):
		self.x, self.y, self.z = self.toCartesian(self.longitude, self.latitude, self.altitude)

	def __init__(self, latitude, longitude, altitude=0.0):
		self._longitude = longitude
		self._latitude = latitude
		self._altitude = altitude
		self.updateECEF();

	@property
	def longitude(self):
		return self._longitude
	
	@longitude.setter
	def longitude(self, value):
		self._longitude = value
		self.updateECEF()

	@property
	def latitude(self):
		return self._latitude
	
	@latitude.setter
	def latitude(self, value):
		self._latitude = value
		self.updateECEF()

	@property
	def altitude(self):
		return self._altitude
	
	@altitude.setter
	def altitude(self, value):
		self._altitude = value
		self.updateECEF()


def main():
	pass

if __name__ == '__main__':
	main()